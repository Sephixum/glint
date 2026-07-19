#include "tests/g_test_common.h"
#include "base/base.h"

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

static string sandbox_path(void)
{
	return string_lit("build/test_sandbox_log");
}

enum
{
	LOG_TRACE = 0x2,
	LOG_DEBUG = 0x4,
	LOG_INFO  = 0x8,
	LOG_WARN  = 0x10,
	LOG_ERROR = 0x20,
	LOG_FATAL = 0x40,
};

// ── stderr capture via fork + pipe ──────────────────────────────────────

static void run_capturing_stderr(char *out, u64 out_cap, void (*body)(void))
{
	int p[2];
	pipe(p);
	pid_t pid = fork();
	if (pid == 0)
	{
		close(p[0]);
		dup2(p[1], STDERR_FILENO);
		close(p[1]);
		body();
		fflush(stderr);
		_exit(0);
	}
	close(p[1]);
	u64 off = 0;
	for (;;)
	{
		if (off >= out_cap - 1)
			break;
		ssize_t n = read(p[0], out + off, out_cap - 1 - off);
		if (n <= 0)
			break;
		off += (u64)n;
	}
	out[off] = 0;
	close(p[0]);
	int status = 0;
	waitpid(pid, &status, 0);
}

// ── log_get / state ─────────────────────────────────────────────────────

static void test_log_get_returns_named_logger(void)
{
	GLINT_TEST_SECTION("log_get returns a named logger");
	logger *a = log_get(string_lit("alpha"));
	GLINT_TEST_CHECK(string_match(log_name(a), string_lit("alpha"), 0));
}

static void test_log_get_caches_same_name(void)
{
	GLINT_TEST_SECTION("log_get caches by name (idempotent)");
	logger *first = log_get(string_lit("cache_test"));
	logger *again = log_get(string_lit("cache_test"));
	GLINT_TEST_CHECK(first == again);
}

static void test_log_get_distinct_names_distinct_loggers(void)
{
	GLINT_TEST_SECTION("log_get yields distinct loggers for distinct names");
	logger *x = log_get(string_lit("xlogger"));
	logger *y = log_get(string_lit("ylogger"));
	GLINT_TEST_CHECK(x != y);
}

// ── runtime filter (direct log_write, bypassing the compile-time guard) ─

// BUG: TRACE/DEBUG/INFO macros are gated by `#if (log_level_trace &
// GLINT_LOG_ENABLE)` in the header, but log_level_trace etc. are C enum
// values, not preprocessor constants. In `#if` they evaluate as 0, so the
// #else path always fires → the macros expand to g_no_op.
//
// To test the runtime filter we call log_write directly.

static void write_info_to_runtime_filter(void)
{
	logger *l = log_get(string_lit("runtime_filt"));
	log_write(l, log_level_info, __FILE__, __LINE__, __func__, "info-payload");
}

static void test_log_filter_suppresses_disabled_level(void)
{
	GLINT_TEST_SECTION("runtime filter suppresses levels not in filter mask");

	log_set_level(log_get(string_lit("runtime_filt")), LOG_WARN);

	char out[4096];
	run_capturing_stderr(out, sizeof(out), write_info_to_runtime_filter);

	// Filter is WARN-only; INFO is masked out.
	GLINT_TEST_CHECK(strstr(out, "info-payload") == 0);
}

static void write_warn_to_runtime_filter(void)
{
	logger *l = log_get(string_lit("warn_filt"));
	log_write(l, log_level_warn, __FILE__, __LINE__, __func__, "warn-on");
}

static void test_log_filter_allows_enabled_level(void)
{
	GLINT_TEST_SECTION("runtime filter passes enabled levels");

	log_set_level(log_get(string_lit("warn_filt")), LOG_WARN);

	char out[4096];
	run_capturing_stderr(out, sizeof(out), write_warn_to_runtime_filter);
	GLINT_TEST_CHECK(strstr(out, "warn-on") != 0);
	// log_level_label was off-by-one (switch started at g_bit(0), enum at
	// g_bit(1)); warn used to render as "ERROR". Now it must say "WARN".
	// The label field is %-5s, so "WARN" is padded to "WARN ".
	GLINT_TEST_CHECK(strstr(out, "[WARN ]") != 0);
	GLINT_TEST_CHECK(strstr(out, "[ERROR]") == 0);
}

// ── unconditional macros (WARN / ERROR) — always emit because they are
//    not wrapped in `#if`.  These macros work today. ────────────────────

static void write_unconditional_macros(void)
{
	logger *l = log_get(string_lit("uncond_test"));
	log_set_level(l, LOG_WARN | LOG_ERROR);
	GLINT_LOG_WARN(l, "uc-warn-baggage");
	GLINT_LOG_ERROR(l, "uc-err-baggage %d", 42);
}

static void test_log_unconditional_macros_fire(void)
{
	GLINT_TEST_SECTION("GLINT_LOG_WARN / ERROR always fire");

	char out[4096];
	run_capturing_stderr(out, sizeof(out), write_unconditional_macros);

	GLINT_TEST_CHECK(strstr(out, "uc-warn-baggage") != 0);
	GLINT_TEST_CHECK(strstr(out, "uc-err-baggage 42") != 0);
}

// ── compile-time gated macros (TRACE / DEBUG / INFO) — now fire when enabled ─

static void write_gated_macros(void)
{
	logger *l = log_get(string_lit("gated"));
	log_set_level(l, LOG_TRACE | LOG_DEBUG | LOG_INFO);
	GLINT_LOG_TRACE(l, "sneaky-trace");
	GLINT_LOG_DEBUG(l, "sneaky-debug");
	GLINT_LOG_INFO(l, "sneaky-info");
}

static void test_log_gated_macros_fire(void)
{
	GLINT_TEST_SECTION("TRACE / DEBUG / INFO macros fire when enabled and filter allows");

	// Three full log lines (timestamp + coloured label + name + location)
	// exceed 256 bytes, so use a generous buffer with the read-loop helper.
	char out[8192];
	run_capturing_stderr(out, sizeof(out), write_gated_macros);

	GLINT_TEST_CHECK(strstr(out, "sneaky-trace") != 0);
	GLINT_TEST_CHECK(strstr(out, "sneaky-debug") != 0);
	GLINT_TEST_CHECK(strstr(out, "sneaky-info") != 0);
	// Labels are %-5s: TRACE/DEBUG are exactly 5 wide, INFO pads to "INFO ".
	GLINT_TEST_CHECK(strstr(out, "[TRACE]") != 0);
	GLINT_TEST_CHECK(strstr(out, "[DEBUG]") != 0);
	GLINT_TEST_CHECK(strstr(out, "[INFO ]") != 0);
}

// ── file output (plain, no-colour, via unconditional macros) ────────────

static void write_to_log_file(void)
{
	logger *l = log_get(string_lit("filechan"));
	log_set_level(l, LOG_WARN | LOG_ERROR);
	GLINT_LOG_WARN(l, "file-warn-word");
	GLINT_LOG_ERROR(l, "file-err-word");
	log_close_file();
}

static void test_log_file_write_plain_content(void)
{
	GLINT_TEST_SECTION("log_open_file writes plain (no-colour) content");

	arena *a = arena_create_default(.name = "log_file_test");
	os_dir_remove_recursive(sandbox_path());
	os_dir_create_recursive(sandbox_path());

	string path = string_f(a, "%.*s/log_file_test_run.log", string_varg(sandbox_path()));
	log_open_file(path);
	write_to_log_file();

	string content_res =
		os_file_read_entire(a, os_file_open(path, (os_file_mode)os_file_mode_read | os_file_mode_write));

	GLINT_TEST_CHECK(string_contains(content_res, string_lit("file-warn-word"), 0));
	GLINT_TEST_CHECK(string_contains(content_res, string_lit("file-err-word"), 0));
	GLINT_TEST_CHECK(string_contains(content_res, string_lit("filechan"), 0));
	GLINT_TEST_CHECK(!string_contains(content_res, string_lit("\033["), 0));

	os_dir_remove(sandbox_path());
	arena_destroy(a);
}

// ── timestamp prefix ────────────────────────────────────────────────────

static void log_timestamp_check(void)
{
	logger *l = log_get(string_lit("tscheck"));
	GLINT_LOG_WARN(l, "ts-body");
}

static void test_log_timestamp_prefix_format(void)
{
	GLINT_TEST_SECTION("log lines use [HH:MML:SS.mmm] timestamp");

	char out[4096];
	run_capturing_stderr(out, sizeof(out), log_timestamp_check);

	char *start = strchr(out, '[');
	GLINT_TEST_CHECK(start != 0);
	if (!start)
	{
		return;
	}
	char *colon = strchr(start, ':');
	GLINT_TEST_CHECK(colon != 0 && colon > start);
	char *dot = strchr(start, '.');
	GLINT_TEST_CHECK(dot != 0 && dot > start);
	char *end = strchr(start, ']');
	GLINT_TEST_CHECK(end != 0 && end > dot);
}

// ── location suffix ─────────────────────────────────────────────────────

static void log_location_trigger(void)
{
	logger *ll = log_get(string_lit("loc"));
	GLINT_LOG_ERROR(ll, "loc-body");
}

static void test_log_location_suffix_appears(void)
{
	GLINT_TEST_SECTION("log lines carry file:line (func) suffix");

	char out[1024];
	run_capturing_stderr(out, sizeof(out), log_location_trigger);
	GLINT_TEST_CHECK(strstr(out, "g_test_log.c") != 0);
	GLINT_TEST_CHECK(strstr(out, "(log_location_trigger)") != 0);
}

// ── long message overflow safety ───────────────────────────────────────

static void log_big_message_trigger(void)
{
	logger *l = log_get(string_lit("biggy"));
	char	payload[8000];
	for (u64 i = 0; i < sizeof(payload) - 1; i++)
		payload[i] = 'A' + (char)(i % 26);
	payload[sizeof(payload) - 1] = 0;
	GLINT_LOG_ERROR(l, "%s", payload);
}

static void test_log_long_message_no_overflow(void)
{
	GLINT_TEST_SECTION("very long message is truncated, no stack overflow");

	char out[9000];
	run_capturing_stderr(out, sizeof(out), log_big_message_trigger);
	GLINT_TEST_CHECK(strlen(out) < 8000);
	GLINT_TEST_CHECK(strchr(out, '[') != 0);
}

// ── null logger safety ──────────────────────────────────────────────────

static void write_null_logger_message(void)
{
	log_write(NULL, LOG_WARN, __FILE__, __LINE__, __func__, "BOING");
}

static void test_log_write_null_logger_is_noop(void)
{
	GLINT_TEST_SECTION("log_write with a null logger is a no-op");

	char out[1024];
	run_capturing_stderr(out, sizeof(out), write_null_logger_message);
	GLINT_TEST_CHECK(out[0] == 0);
}

// ── close() safety ──────────────────────────────────────────────────────

static void test_log_close_file_when_unopened(void)
{
	GLINT_TEST_SECTION("log_close_file is safe when no file is open");
	log_close_file();
	log_close_file();
	GLINT_TEST_CHECK(1);
}

// ── fatal abort ─────────────────────────────────────────────────────────

static void child_log_fatal(void)
{
	logger *l = log_get(string_lit("fatalchan"));
	log_set_level(l, LOG_FATAL);
	GLINT_LOG_FATAL(l, "kaboom");
	_exit(1); // unreachable
}

static void test_log_fatal_aborts_process(void)
{
	GLINT_TEST_SECTION("GLINT_LOG_FATAL sends abort");

	int p[2];
	pipe(p);
	pid_t child = fork();
	if (child == 0)
	{
		close(p[0]);
		dup2(p[1], STDERR_FILENO);
		close(p[1]);
		child_log_fatal();
		_exit(42);
	}
	close(p[1]);
	char drain[4096];
	while (read((p[0]), drain, sizeof(drain)) > 0 /* empty */)
		;
	close(p[0]);

	int status;
	int wait_status = waitpid(child, &status, 0);
	GLINT_TEST_CHECK(WIFSIGNALED(status));
	GLINT_TEST_CHECK_EQ_INT(WTERMSIG(status), SIGABRT);
}

// ── registration ─────────────────────────────────────────────────────────

static void register_log_tests(void) __attribute__((constructor));
static void register_log_tests(void)
{
	GLINT_TEST_REGISTER(test_log_get_returns_named_logger);
	GLINT_TEST_REGISTER(test_log_get_caches_same_name);
	GLINT_TEST_REGISTER(test_log_get_distinct_names_distinct_loggers);
	GLINT_TEST_REGISTER(test_log_filter_suppresses_disabled_level);
	GLINT_TEST_REGISTER(test_log_filter_allows_enabled_level);
	GLINT_TEST_REGISTER(test_log_gated_macros_fire);
	GLINT_TEST_REGISTER(test_log_unconditional_macros_fire);
	GLINT_TEST_REGISTER(test_log_file_write_plain_content);
	GLINT_TEST_REGISTER(test_log_timestamp_prefix_format);
	GLINT_TEST_REGISTER(test_log_location_suffix_appears);
	GLINT_TEST_REGISTER(test_log_long_message_no_overflow);
	GLINT_TEST_REGISTER(test_log_write_null_logger_is_noop);
	GLINT_TEST_REGISTER(test_log_close_file_when_unopened);
	GLINT_TEST_REGISTER(test_log_fatal_aborts_process);
}

#include "base/base.c"
