#include "tests/g_test_common.h"
#include "base/base.h"

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// ── helpers ───────────────────────────────────────────────────────────────

#define CHECK_STRING_EQ(s, expected_lit)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		string _exp = string_lit(expected_lit);                                                                        \
		if ((s).size == _exp.size && memcmp((s).str, _exp.str, _exp.size) == 0)                                        \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr,                                                                                            \
					"[FAIL] %s:%d: strings differ (\"%.*s\" != \"%.*s\")\n",                                           \
					__FILE__,                                                                                          \
					__LINE__,                                                                                          \
					(int)(s).size,                                                                                     \
					(s).str,                                                                                           \
					(int)_exp.size,                                                                                    \
					_exp.str);                                                                                         \
		}                                                                                                              \
	} while (0)

// Root sandbox under build/ — created from a clean slate each run.
static string sandbox_path(void)
{
	return string_lit("build/test_sandbox_os");
}

// ── system_info ────────────────────────────────────────────────────────────

static void test_os_system_info(void)
{
	GLINT_TEST_SECTION("os_get_system_info");

	system_info *info = os_get_system_info();
	GLINT_TEST_CHECK(info != 0);

	// Logical processor count must be positive on any real machine.
	GLINT_TEST_CHECK(info->logical_processor_count > 0);

	// Page size is a power of two and at least the conventional 4 KiB.
	GLINT_TEST_CHECK(info->page_size >= 4096);
	GLINT_TEST_CHECK((info->page_size & (info->page_size - 1)) == 0);

	// allocation_granularity is a multiple of page_size on every platform
	// that exposes it (on Linux the impl aliases it to page_size).
	GLINT_TEST_CHECK(info->allocation_granularity >= info->page_size);
	GLINT_TEST_CHECK((info->allocation_granularity % info->page_size) == 0);

	// large_page_size is >= page_size.
	GLINT_TEST_CHECK(info->large_page_size >= info->page_size);

	// machine_name is null-terminated inside its fixed buffer.
	GLINT_TEST_CHECK(info->machine_name[0] != 0);

	// Calling twice must return the same cached pointer.
	GLINT_TEST_CHECK(os_get_system_info() == info);
}

// ── memory ─────────────────────────────────────────────────────────────────

static void test_os_memory_reserve_commit_write_release(void)
{
	GLINT_TEST_SECTION("os_memory reserve/commit/write/release");

	u64	  page = os_get_system_info()->page_size;
	void *p	   = os_memory_reserve(page * 2);
	GLINT_TEST_CHECK(p != 0);

	b8 committed = os_memory_commit(p, page * 2);
	if (committed)
	{
		// Touching the committed range must be safe and read back what we write.
		u8 *bytes = (u8 *)p;
		for (u64 i = 0; i < page * 2; i++)
			bytes[i] = (u8)(i & 0xff);
		GLINT_TEST_CHECK(bytes[0] == 0);
		GLINT_TEST_CHECK(bytes[page] == 0); // second page writable too
		GLINT_TEST_CHECK(bytes[page * 2 - 1] == (u8)((page * 2 - 1) & 0xff));

		// Decommit returns the pages to the OS — we must not touch them after.
		os_memory_decommit(p, page * 2);
		GLINT_TEST_CHECK(1); // reached without crashing
	}

	os_memory_release(p, page * 2);
	GLINT_TEST_CHECK(1); // released cleanly
}

static void test_os_memory_reserve_zero_size(void)
{
	GLINT_TEST_SECTION("os_memory reserve(0)");

	GLINT_TEST_CHECK(os_memory_reserve(0) == 0);
	GLINT_TEST_CHECK(os_memory_reserve_large(0) == 0);
}

static void test_os_memory_large_pages(void)
{
	GLINT_TEST_SECTION("os_memory large pages");

	u64 large = os_get_system_info()->large_page_size;
	// Reserve 2 large pages. MAP_HUGETLB will fail on systems without huge
	// pages reserved, in which case reserve returns 0 — that is correct
	// behavior, so we treat it as a graceful skip and still pass.
	void *p = os_memory_reserve_large(large * 2);
	if (p == 0)
	{
		GLINT_TEST_CHECK(1); // large pages unavailable — skipped
		return;
	}

	b8 committed = os_memory_commit_large(p, large * 2);
	GLINT_TEST_CHECK(committed != 0);
	if (committed)
	{
		*(u64 *)p = 0xDEADBEEF;
		GLINT_TEST_CHECK(*(u64 *)p == 0xDEADBEEF);
	}
	os_memory_release(p, large * 2);
	GLINT_TEST_CHECK(1);
}

// ── time ────────────────────────────────────────────────────────────────────

static void test_os_time_now_and_monotonic(void)
{
	GLINT_TEST_SECTION("os_time now/monotonic");

	os_time wall = os_time_now();
	GLINT_TEST_CHECK(wall.seconds > 0);

	os_time m0 = os_time_monotonic();
	os_thread_sleep(20); // milliseconds
	os_time m1 = os_time_monotonic();

	// Monotonic clock must not go backwards.
	GLINT_TEST_CHECK(m1.seconds > m0.seconds || (m1.seconds == m0.seconds && m1.nanoseconds >= m0.nanoseconds));

	os_time delta = os_time_sub(m1, m0);
	u64		ms	  = os_time_as_ms(delta);
	// Allow generous slack for scheduler jitter; must be >= ~10ms after a 20ms sleep.
	GLINT_TEST_CHECK(ms >= 10);

	// as_us must be >= as_ms * 1000 within rounding (nanosecond -> us is finer).
	GLINT_TEST_CHECK(os_time_as_us(delta) >= ms * 1000);
}

static void test_os_time_sub_and_units(void)
{
	GLINT_TEST_SECTION("os_time sub / as_ms / as_us");

	os_time a = {3, 750'000'000};
	os_time b = {1, 250'000'000};
	os_time d = os_time_sub(a, b);
	GLINT_TEST_CHECK_EQ_U64(d.seconds, 2);
	GLINT_TEST_CHECK_EQ_U64(d.nanoseconds, 500'000'000);
	GLINT_TEST_CHECK_EQ_U64(os_time_as_ms(d), 2500);
	GLINT_TEST_CHECK_EQ_U64(os_time_as_us(d), 2'500'000);

	// nanosecond borrow across a second boundary: 1.0s - 0.5s = 0.5s
	os_time x = {2, 0};
	os_time y = {1, 500'000'000};
	os_time r = os_time_sub(x, y);
	GLINT_TEST_CHECK_EQ_U64(r.seconds, 0);
	GLINT_TEST_CHECK_EQ_U64(r.nanoseconds, 500'000'000);

	// a < b => zero (clamped).
	os_time lt = os_time_sub(y, x);
	GLINT_TEST_CHECK_EQ_U64(lt.seconds, 0);
	GLINT_TEST_CHECK_EQ_U64(lt.nanoseconds, 0);
}

// ── os_date (broken-down calendar from os_time) ────────────────────────────

static void test_os_time_to_date_utc_fixed_epochs(void)
{
	GLINT_TEST_SECTION("os_time_to_date_utc on fixed epochs");

	// Unix epoch 0 = 1970-01-01 00:00:00 UTC, a Thursday.
	os_date d0 = os_time_to_date_utc((os_time){0, 0});
	GLINT_TEST_CHECK(d0.year == 1970);
	GLINT_TEST_CHECK(d0.month == 1);
	GLINT_TEST_CHECK(d0.day == 1);
	GLINT_TEST_CHECK(d0.hour == 0);
	GLINT_TEST_CHECK(d0.minute == 0);
	GLINT_TEST_CHECK(d0.second == 0);
	GLINT_TEST_CHECK(d0.nanosecond == 0);
	GLINT_TEST_CHECK(d0.weekday == os_weekday_thursday);
	GLINT_TEST_CHECK(d0.day_of_year == 1);
	GLINT_TEST_CHECK(d0.utc_offset_seconds == 0);
	GLINT_TEST_CHECK(d0.is_dst == 0);

	// 2000-01-01 00:00:00 UTC = a Saturday. Seconds since epoch:
	// 30 years × 365 days + 7 leap days = 10957 days × 86400.
	os_date ds = os_time_to_date_utc((os_time){10957ULL * 86400, 0});
	GLINT_TEST_CHECK(ds.year == 2000);
	GLINT_TEST_CHECK(ds.month == 1);
	GLINT_TEST_CHECK(ds.day == 1);
	GLINT_TEST_CHECK(ds.weekday == os_weekday_saturday);
	GLINT_TEST_CHECK(ds.utc_offset_seconds == 0);
}

static void test_os_time_to_date_local_ranges(void)
{
	GLINT_TEST_SECTION("os_time_to_date returns valid calendar ranges");

	os_date d = os_time_to_date(os_time_now());

	// Every field must be within its defined range.
	GLINT_TEST_CHECK(d.year >= 2025 && d.year <= 2100);
	GLINT_TEST_CHECK(d.month >= 1 && d.month <= 12);
	GLINT_TEST_CHECK(d.day >= 1 && d.day <= 31);
	GLINT_TEST_CHECK(d.hour <= 23);
	GLINT_TEST_CHECK(d.minute <= 59);
	GLINT_TEST_CHECK(d.second <= 59);
	GLINT_TEST_CHECK(d.nanosecond < 1'000'000'000);
	GLINT_TEST_CHECK(d.weekday >= os_weekday_sunday && d.weekday <= os_weekday_saturday);
	GLINT_TEST_CHECK(d.day_of_year >= 1 && d.day_of_year <= 366);
	// is_dst and utc_offset_seconds have no universal invariant, but utc offset
	// is bounded on Earth: -12h to +14h in seconds.
	GLINT_TEST_CHECK(d.utc_offset_seconds >= -43200 && d.utc_offset_seconds <= 50400);
}

static void test_os_date_utc_vs_local_consistency(void)
{
	GLINT_TEST_SECTION("UTC and local date differ only by utc offset");

	os_time now = os_time_now();
	os_date utc  = os_time_to_date_utc(now);
	os_date loc  = os_time_to_date(now);

	// UTC offset must be 0, local must not be 0 unless local TZ is UTC.
	GLINT_TEST_CHECK_EQ_U64((u32)utc.utc_offset_seconds, 0);

	// Same epoch — year/month/day differ at most by 1 near midnight boundaries.
	GLINT_TEST_CHECK(loc.weekday == utc.weekday || loc.weekday == ((utc.weekday + 1) % 7) || loc.weekday == ((utc.weekday + 6) % 7));
}

// ── file ─────────────────────────────────────────────────────────────────────

static void test_os_file_open_invalid(void)
{
	GLINT_TEST_SECTION("os_file open invalid");

	string	bad = string_lit("build/test_sandbox_os/does_not_exist_XXXX.txt");
	os_file f	= os_file_open(bad, os_file_mode_read);
	GLINT_TEST_CHECK(f.opaque[0] == 0); // failed open yields a null handle
	GLINT_TEST_CHECK(!os_file_valid(f));
}

static void test_os_file_write_read_entire(void)
{
	GLINT_TEST_SECTION("os_file write/read/read_entire/size/flush");

	arena *a = arena_create_default(.name = "os_test_file");

	os_dir_remove_recursive(sandbox_path());
	os_dir_create_recursive(sandbox_path());

	string		path	= string_f(a, "%.*s/roundtrip.txt", string_varg(sandbox_path()));
	char const *payload = "hello glint os layer\n";
	char const *extra	= "second line\n";

	os_file f = os_file_open(path, os_file_mode_read | os_file_mode_write);
	GLINT_TEST_CHECK(f.opaque[0] != 0);
	GLINT_TEST_CHECK(os_file_valid(f));

	u64 n1 = os_file_write(f, payload, (u64)strlen(payload));
	GLINT_TEST_CHECK_EQ_U64(n1, (u64)strlen(payload));
	GLINT_TEST_CHECK(os_file_flush(f));

	u64 n2 = os_file_write(f, extra, (u64)strlen(extra));
	GLINT_TEST_CHECK_EQ_U64(n2, (u64)strlen(extra));

	GLINT_TEST_CHECK_EQ_U64(os_file_size(f), (u64)(strlen(payload) + strlen(extra)));

	os_file_close(f);

	// Re-open read-only and read the entire thing back.
	os_file r = os_file_open(path, os_file_mode_read);
	GLINT_TEST_CHECK(r.opaque[0] != 0);
	string contents = os_file_read_entire(a, r);
	CHECK_STRING_EQ(contents, "hello glint os layer\nsecond line\n");
	os_file_close(r);

	// Partial read of a bounded size.
	os_file r2	 = os_file_open(path, os_file_mode_read);
	string	head = os_file_read(a, r2, 5);
	CHECK_STRING_EQ(head, "hello");
	os_file_close(r2);

	// Two-arg write must append (we opened RW which is O_CREAT|O_RDWR; verify
	// the file persisted on disk through os_path_get_info).
	os_path_info info = os_path_get_info(path);
	GLINT_TEST_CHECK(info.exists);
	GLINT_TEST_CHECK(!info.is_dir);
	GLINT_TEST_CHECK_EQ_U64(info.size_bytes, (u64)(strlen(payload) + strlen(extra)));

	os_dir_remove_recursive(sandbox_path());
	arena_destroy(a);
}

static void test_os_file_error_propagation(void)
{
	GLINT_TEST_SECTION("os_file error string");

	// Trigger a failure then read back the OS error string.
	os_file f = os_file_open(string_lit("build/test_sandbox_os/never_here.txt"), os_file_mode_read);
	GLINT_TEST_CHECK(f.opaque[0] == 0);
	string err = os_file_error();
	GLINT_TEST_CHECK(err.size > 0);
}

// ── directories ─────────────────────────────────────────────────────────────

static void test_os_dir_create_list_remove(void)
{
	GLINT_TEST_SECTION("os_dir create/list/remove");

	arena *a = arena_create_default(.name = "os_test_dir");

	os_dir_remove_recursive(sandbox_path());
	os_dir_create_recursive(sandbox_path());

	string root = sandbox_path();
	GLINT_TEST_CHECK(os_path_get_info(root).exists);

	// Create a few entries: two files and one subdir.
	os_file f1 = os_file_open(string_f(a, "%.*s/a.txt", string_varg(root)), os_file_mode_write);
	GLINT_TEST_CHECK(f1.opaque[0] != 0);
	os_file_write(f1, "a", 1);
	os_file_close(f1);

	os_file f2 = os_file_open(string_f(a, "%.*s/b.txt", string_varg(root)), os_file_mode_write);
	GLINT_TEST_CHECK(f2.opaque[0] != 0);
	os_file_write(f2, "bb", 2);
	os_file_close(f2);

	GLINT_TEST_CHECK(os_dir_create(string_f(a, "%.*s/sub", string_varg(root))));
	GLINT_TEST_CHECK(os_path_get_info(string_f(a, "%.*s/sub", string_varg(root))).is_dir);

	// Listing should report exactly {a.txt, b.txt, sub}.
	string_list list = os_dir_list(a, root);
	GLINT_TEST_CHECK_EQ_INT((int)list.node_count, 3);

	// Verify expected entries are present (order-independent).
	b8 saw_a = 0, saw_b = 0, saw_sub = 0;
	for (string_node *n = list.first; n != 0; n = n->next)
	{
		if (string_match(n->string, string_lit("a.txt"), 0))
			saw_a = 1;
		else if (string_match(n->string, string_lit("b.txt"), 0))
			saw_b = 1;
		else if (string_match(n->string, string_lit("sub"), 0))
			saw_sub = 1;
	}
	GLINT_TEST_CHECK(saw_a && saw_b && saw_sub);

	// Clear a single directory entry (sub is empty so rmdir works).
	GLINT_TEST_CHECK(os_dir_remove(string_f(a, "%.*s/sub", string_varg(root))));
	GLINT_TEST_CHECK(!os_path_get_info(string_f(a, "%.*s/sub", string_varg(root))).exists);

	// Recursive removal of the whole sandbox, even with files left behind.
	GLINT_TEST_CHECK(os_dir_remove_recursive(sandbox_path()));
	GLINT_TEST_CHECK(!os_path_get_info(sandbox_path()).exists);

	arena_destroy(a);
}

// ── path queries ────────────────────────────────────────────────────────────

static void test_os_path_get_type(void)
{
	GLINT_TEST_SECTION("os_path_get_type");

	arena *a = arena_create_default(.name = "os_test_path");
	os_dir_remove_recursive(sandbox_path());
	os_dir_create_recursive(sandbox_path());

	string root = sandbox_path();
	GLINT_TEST_CHECK(os_path_get_type(root) == os_path_type_dir);
	GLINT_TEST_CHECK(os_path_get_type(string_lit("build/test_sandbox_os/missing")) == os_path_type_none);

	string	fp = string_f(a, "%.*s/file.bin", string_varg(root));
	os_file f  = os_file_open(fp, os_file_mode_write);
	GLINT_TEST_CHECK(f.opaque[0] != 0);
	os_file_write(f, "data", 4);
	os_file_close(f);
	GLINT_TEST_CHECK(os_path_get_type(fp) == os_path_type_file);

	// Symlink: create a link to the file. Use a target relative to the
	// link's directory (just the bare filename) so the kernel resolves
	// it correctly. Absolute or fully-qualified relative paths would be
	// incorrectly resolved relative to the symlink's own directory.
	string link = string_f(a, "%.*s/link.bin", string_varg(root));
	GLINT_TEST_CHECK(os_symlink_create(string_lit("file.bin"), link));
	GLINT_TEST_CHECK(os_path_get_type(link) == os_path_type_symlink);

	// os_path_get_info follows the link (stat); it sees the target file:
	// exists=true, is_symlink=0 (stat, not lstat), size_bytes=4.
	os_path_info info = os_path_get_info(link);
	GLINT_TEST_CHECK(info.exists);
	GLINT_TEST_CHECK_EQ_U64(info.size_bytes, 4);

	os_dir_remove_recursive(sandbox_path());
	arena_destroy(a);
}

static void test_os_symlink_remove(void)
{
	GLINT_TEST_SECTION("os_symlink create/remove");

	arena *a = arena_create_default(.name = "os_test_sym");
	os_dir_remove_recursive(sandbox_path());
	os_dir_create_recursive(sandbox_path());

	string	root   = sandbox_path();
	string	target = string_f(a, "%.*s/target.txt", string_varg(root));
	os_file f	   = os_file_open(target, os_file_mode_write);
	GLINT_TEST_CHECK(f.opaque[0] != 0);
	os_file_write(f, "t", 1);
	os_file_close(f);

	string link = string_f(a, "%.*s/alias.txt", string_varg(root));
	GLINT_TEST_CHECK(os_symlink_create(string_lit("target.txt"), link));
	GLINT_TEST_CHECK(os_path_get_type(link) == os_path_type_symlink);

	GLINT_TEST_CHECK(os_symlink_remove(link));
	GLINT_TEST_CHECK(os_path_get_type(link) == os_path_type_none);

	// Target should still exist (we only removed the link).
	GLINT_TEST_CHECK(os_path_get_type(target) == os_path_type_file);

	os_dir_remove_recursive(sandbox_path());
	arena_destroy(a);
}

// ── library ─────────────────────────────────────────────────────────────────

static void test_os_library_load_symbol_unload(void)
{
	GLINT_TEST_SECTION("os_library load/symbol/unload");

	// libm.so.6 ships with glibc on Linux; dlopen + dlsym(sqrt) is portable.
	os_library lib = os_library_load(string_lit("libm.so.6"));
	GLINT_TEST_CHECK(lib.opaque[0] != 0);
	if (lib.opaque[0] == 0)
		return; // can't continue meaningfully

	void *sym = os_library_symbol(lib, string_lit("sqrt"));
	GLINT_TEST_CHECK(sym != 0);
	if (sym == 0)
	{
		os_library_unload(lib);
		return;
	}

	double (*pf_sqrt)(double) = (double (*)(double))sym;
	GLINT_TEST_CHECK_EQ_FLOAT(pf_sqrt(25.0), 5.0, 1e-9);
	GLINT_TEST_CHECK_EQ_FLOAT(pf_sqrt(2.0), 1.41421356237, 1e-6);

	// Looking up a bogus symbol must return null without crashing.
	GLINT_TEST_CHECK(os_library_symbol(lib, string_lit("glint_does_not_exist_xyz")) == 0);

	os_library_unload(lib);
	GLINT_TEST_CHECK(1);
}

// ── registration ────────────────────────────────────────────────────────────

static void register_os_tests(void) __attribute__((constructor));
static void register_os_tests(void)
{
	GLINT_TEST_REGISTER(test_os_system_info);
	GLINT_TEST_REGISTER(test_os_memory_reserve_commit_write_release);
	GLINT_TEST_REGISTER(test_os_memory_reserve_zero_size);
	GLINT_TEST_REGISTER(test_os_memory_large_pages);
	GLINT_TEST_REGISTER(test_os_time_now_and_monotonic);
	GLINT_TEST_REGISTER(test_os_time_sub_and_units);
	GLINT_TEST_REGISTER(test_os_time_to_date_utc_fixed_epochs);
	GLINT_TEST_REGISTER(test_os_time_to_date_local_ranges);
	GLINT_TEST_REGISTER(test_os_date_utc_vs_local_consistency);
	GLINT_TEST_REGISTER(test_os_file_open_invalid);
	GLINT_TEST_REGISTER(test_os_file_write_read_entire);
	GLINT_TEST_REGISTER(test_os_file_error_propagation);
	GLINT_TEST_REGISTER(test_os_dir_create_list_remove);
	GLINT_TEST_REGISTER(test_os_path_get_type);
	GLINT_TEST_REGISTER(test_os_symlink_remove);
	GLINT_TEST_REGISTER(test_os_library_load_symbol_unload);
}

#include "base/base.c"
