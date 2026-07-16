#include "../g_test_common.h"
#include "../../base/core/g_platform_and_compiler_defines.h"
#include "../../base/string/g_string.h"
#include "../../base/memory/g_arena.h"

static void test_operating_system_to_string(void)
{
	GLINT_TEST_SECTION("operating_system to string");
	
	operating_system os_null = operating_system_null;
	string str_null = string_from_g_operating_system(os_null);
	GLINT_TEST_CHECK(string_match(str_null, string_lit_comp("null"), 0));
	
	operating_system os_windows = operating_system_windows;
	string str_windows = string_from_g_operating_system(os_windows);
	GLINT_TEST_CHECK(string_match(str_windows, string_lit_comp("windows"), 0));
	
	operating_system os_linux = operating_system_linux;
	string str_linux = string_from_g_operating_system(os_linux);
	GLINT_TEST_CHECK(string_match(str_linux, string_lit_comp("linux"), 0));
	
	operating_system os_mac = operating_system_mac;
	string str_mac = string_from_g_operating_system(os_mac);
	GLINT_TEST_CHECK(string_match(str_mac, string_lit_comp("mac"), 0));
}

static void test_arena_flags_to_string(void)
{
	GLINT_TEST_SECTION("arena_flags to string");
	
	arena* a = arena_create_default();
	
	arena_flags flags_none = 0;
	string str_none = string_from_arena_flags(a, flags_none);
	GLINT_TEST_CHECK(string_match(str_none, string_lit_comp("none"), 0));
	
	arena_flags flags_no_chain = arena_flag_no_chain;
	string str_no_chain = string_from_arena_flags(a, flags_no_chain);
	GLINT_TEST_CHECK(string_match(str_no_chain, string_lit_comp("no_chain"), 0));
	
	arena_flags flags_large_pages = arena_flag_large_pages;
	string str_large_pages = string_from_arena_flags(a, flags_large_pages);
	GLINT_TEST_CHECK(string_match(str_large_pages, string_lit_comp("large_pages"), 0));
	
	arena_flags flags_both = arena_flag_no_chain | arena_flag_large_pages;
	string str_both = string_from_arena_flags(a, flags_both);
	GLINT_TEST_CHECK(string_match(str_both, string_lit_comp("no_chain, large_pages"), 0));
	
	arena_destroy(a);
}

static void test_string_match_flags_to_string(void)
{
	GLINT_TEST_SECTION("string_match_flags to string");
	
	arena* a = arena_create_default();
	
	string_match_flags flags_none = 0;
	string str_none = string_from_string_match_flags(a, flags_none);
	GLINT_TEST_CHECK(string_match(str_none, string_lit_comp("none"), 0));
	
	string_match_flags flags_case = string_match_flag_case_insensitive;
	string str_case = string_from_string_match_flags(a, flags_case);
	GLINT_TEST_CHECK(string_match(str_case, string_lit_comp("case_insensitive"), 0));
	
	string_match_flags flags_multi = string_match_flag_case_insensitive | string_match_flag_slash_insensitive;
	string str_multi = string_from_string_match_flags(a, flags_multi);
	GLINT_TEST_CHECK(string_find_needle(str_multi, 0, string_lit_comp("case_insensitive"), 0) != str_multi.size);
	GLINT_TEST_CHECK(string_find_needle(str_multi, 0, string_lit_comp("slash_insensitive"), 0) != str_multi.size);
	
	arena_destroy(a);
}

static void test_string_split_flags_to_string(void)
{
	GLINT_TEST_SECTION("string_split_flags to string");
	
	arena* a = arena_create_default();
	
	string_split_flags flags_none = 0;
	string str_none = string_from_string_split_flags(a, flags_none);
	GLINT_TEST_CHECK(string_match(str_none, string_lit_comp("none"), 0));
	
	string_split_flags flags_keep = string_split_flag_keep_empties;
	string str_keep = string_from_string_split_flags(a, flags_keep);
	GLINT_TEST_CHECK(string_match(str_keep, string_lit_comp("keep_empties"), 0));
	
	arena_destroy(a);
}

static void test_path_style_to_string(void)
{
	GLINT_TEST_SECTION("path_style to string");
	
	arena* a = arena_create_default();
	
	path_style style_null = path_style_null;
	string str_null = string_from_path_style(a, style_null);
	GLINT_TEST_CHECK(string_match(str_null, string_lit_comp("null"), 0));
	
	path_style style_relative = path_style_relative;
	string str_relative = string_from_path_style(a, style_relative);
	GLINT_TEST_CHECK(string_match(str_relative, string_lit_comp("relative"), 0));
	
	path_style style_multi = path_style_relative | path_style_unix_absolute;
	string str_multi = string_from_path_style(a, style_multi);
	GLINT_TEST_CHECK(string_find_needle(str_multi, 0, string_lit_comp("relative"), 0) != str_multi.size);
	GLINT_TEST_CHECK(string_find_needle(str_multi, 0, string_lit_comp("unix_absolute"), 0) != str_multi.size);
	
	arena_destroy(a);
}

static void register_enum_to_string_tests(void) __attribute__((constructor));
static void register_enum_to_string_tests(void)
{
	GLINT_TEST_REGISTER(test_operating_system_to_string);
	GLINT_TEST_REGISTER(test_arena_flags_to_string);
	GLINT_TEST_REGISTER(test_string_match_flags_to_string);
	GLINT_TEST_REGISTER(test_string_split_flags_to_string);
	GLINT_TEST_REGISTER(test_path_style_to_string);
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
