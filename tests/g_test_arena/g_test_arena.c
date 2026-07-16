#include "../g_test_common.h"
#include "../../base/core/g_platform_and_compiler_defines.h"
#include "../../base/memory/g_arena.h"

static void test_arena_basic(void)
{
	GLINT_TEST_SECTION("arena basic");

	arena* a = arena_create_default(.name = "test_arena");
	GLINT_TEST_CHECK(a != 0);

	// Test basic allocation
	u64* num = arena_push_struct(a, u64);
	GLINT_TEST_CHECK(num != 0);
	*num = 42;
	GLINT_TEST_CHECK_EQ_INT(*num, 42);

	// Test array allocation
	u64* arr = arena_push_array(a, u64, 10);
	GLINT_TEST_CHECK(arr != 0);
	for (u64 i = 0; i < 10; i++)
	{
		arr[i] = i * i;
	}
	GLINT_TEST_CHECK_EQ_INT(arr[5], 25);

	arena_destroy(a);
}

static void test_arena_temp(void)
{
	GLINT_TEST_SECTION("arena temp");

	arena* a = arena_create_default();

	u64 pos_start = arena_pos(a);

	temp_arena temp = temp_arena_begin(a);

	// Allocate in temp scope
	u64* data = arena_push_array(a, u64, 100);
	GLINT_TEST_CHECK(data != 0);
	u64 pos_after_alloc = arena_pos(a);
	GLINT_TEST_CHECK(pos_after_alloc > pos_start);

	// End temp - should restore position
	temp_arena_end(temp);
	u64 pos_after_end = arena_pos(a);
	GLINT_TEST_CHECK_EQ_INT(pos_after_end, pos_start);

	arena_destroy(a);
}

static void test_arena_scratch_no_conflicts(void)
{
	GLINT_TEST_SECTION("arena scratch no conflicts");

	// Get scratch with no conflicts
	temp_arena scratch = arena_get_scratch(0, 0);
	GLINT_TEST_CHECK(scratch.arena != 0);

	// Use scratch for temporary work
	u64* buffer = arena_push_array(scratch.arena, u64, 50);
	GLINT_TEST_CHECK(buffer != 0);

	for (u64 i = 0; i < 50; i++)
	{
		buffer[i] = i;
	}
	GLINT_TEST_CHECK_EQ_INT(buffer[25], 25);

	arena_release_scratch(scratch);
}

static void test_arena_scratch_with_conflict(void)
{
	GLINT_TEST_SECTION("arena scratch with conflict");

	arena* my_arena = arena_create_default(.name = "my_arena");

	// Get scratch that doesn't conflict with my_arena
	temp_arena scratch = arena_get_scratch(&my_arena, 1);
	GLINT_TEST_CHECK(scratch.arena != 0);
	GLINT_TEST_CHECK(scratch.arena != my_arena);

	// Both arenas should be usable independently
	u64* my_data	  = arena_push_array(my_arena, u64, 10);
	u64* scratch_data = arena_push_array(scratch.arena, u64, 10);

	GLINT_TEST_CHECK(my_data != 0);
	GLINT_TEST_CHECK(scratch_data != 0);
	GLINT_TEST_CHECK(my_data != scratch_data);

	arena_release_scratch(scratch);
	arena_destroy(my_arena);
}

static void test_arena_scratch_nested(void)
{
	GLINT_TEST_SECTION("arena scratch nested");

	// Get first scratch
	temp_arena scratch1 = arena_get_scratch(0, 0);
	GLINT_TEST_CHECK(scratch1.arena != 0);

	// Get second scratch that doesn't conflict with first
	temp_arena scratch2 = arena_get_scratch(&scratch1.arena, 1);
	GLINT_TEST_CHECK(scratch2.arena != 0);
	GLINT_TEST_CHECK(scratch2.arena != scratch1.arena);

	// Both should be usable
	u64* data1 = arena_push_array(scratch1.arena, u64, 5);
	u64* data2 = arena_push_array(scratch2.arena, u64, 5);

	GLINT_TEST_CHECK(data1 != 0);
	GLINT_TEST_CHECK(data2 != 0);

	data1[0] = 111;
	data2[0] = 222;

	GLINT_TEST_CHECK_EQ_INT(data1[0], 111);
	GLINT_TEST_CHECK_EQ_INT(data2[0], 222);

	arena_release_scratch(scratch2);
	arena_release_scratch(scratch1);
}

static void test_arena_scratch_multiple_conflicts(void)
{
	GLINT_TEST_SECTION("arena scratch multiple conflicts");

	arena* arena1 = arena_create_default(.name = "arena1");
	arena* arena2 = arena_create_default(.name = "arena2");

	// Get scratch that doesn't conflict with either
	arena*	   conflicts[] = {arena1, arena2};
	temp_arena scratch	   = arena_get_scratch(conflicts, 2);

	GLINT_TEST_CHECK(scratch.arena != 0);
	GLINT_TEST_CHECK(scratch.arena != arena1);
	GLINT_TEST_CHECK(scratch.arena != arena2);

	arena_release_scratch(scratch);
	arena_destroy(arena2);
	arena_destroy(arena1);
}

static void test_arena_clear(void)
{
	GLINT_TEST_SECTION("arena clear");

	arena* a = arena_create_default();

	// Allocate some data
	u64* data = arena_push_array(a, u64, 100);
	GLINT_TEST_CHECK(data != 0);
	u64 pos_after = arena_pos(a);
	GLINT_TEST_CHECK(pos_after > 0);

	// Clear arena
	arena_clear(a);
	u64 pos_cleared = arena_pos(a);
	GLINT_TEST_CHECK(pos_cleared < pos_after);

	// Should be able to allocate again
	u64* new_data = arena_push_array(a, u64, 50);
	GLINT_TEST_CHECK(new_data != 0);

	arena_destroy(a);
}

static void register_arena_tests(void) __attribute__((constructor));
static void register_arena_tests(void)
{
	GLINT_TEST_REGISTER(test_arena_basic);
	GLINT_TEST_REGISTER(test_arena_temp);
	GLINT_TEST_REGISTER(test_arena_scratch_no_conflicts);
	GLINT_TEST_REGISTER(test_arena_scratch_with_conflict);
	GLINT_TEST_REGISTER(test_arena_scratch_nested);
	GLINT_TEST_REGISTER(test_arena_scratch_multiple_conflicts);
	GLINT_TEST_REGISTER(test_arena_clear);
}

#include "../../base/memory/g_arena.c"

// Include platform-specific OS implementations
#if GLINT_OS_LINUX
#	include "../../base/os/platform/linux/g_os_memory_linux.c"
#	include "../../base/os/platform/linux/g_os_system_info_linux.c"
#elif GLINT_OS_WINDOWS
#	include "../../base/os/platform/windows/g_os_memory_windows.c"
#	include "../../base/os/platform/windows/g_os_system_info_windows.c"
#elif GLINT_OS_MAC
// macOS implementations would go here
#	error "macOS OS implementations not yet added"
#else
#	error "Unsupported OS"
#endif
