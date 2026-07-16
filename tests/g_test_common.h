#ifndef GLINT_TEST_COMMON_H
#define GLINT_TEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

typedef struct test_state
{
	int passed;
	int failed;
	int current_suite_failed; // reset per-suite, lets you bail a suite early if you want
} test_state;

// Declare global test stats (defined in g_test_main.c)
extern test_state global_test_stats;

// ---- basic checks --------------------------------------------------------

#define GLINT_TEST_CHECK(cond)                                                                                         \
	do                                                                                                                 \
	{                                                                                                                  \
		if (cond)                                                                                                      \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond);                                          \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_MSG(cond, ...)                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		if (cond)                                                                                                      \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr, "[FAIL] %s:%d: ", __FILE__, __LINE__);                                                     \
			fprintf(stderr, __VA_ARGS__);                                                                              \
			fprintf(stderr, "\n");                                                                                     \
		}                                                                                                              \
	} while (0)

// ---- typed equality checks ------------------------------------------------

#define GLINT_TEST_CHECK_EQ_INT(a, b)                                                                                  \
	do                                                                                                                 \
	{                                                                                                                  \
		long long _a = (long long)(a), _b = (long long)(b);                                                            \
		if (_a == _b)                                                                                                  \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr, "[FAIL] %s:%d: %s (%lld) != %s (%lld)\n", __FILE__, __LINE__, #a, _a, #b, _b);             \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_EQ_U64(a, b)                                                                                  \
	do                                                                                                                 \
	{                                                                                                                  \
		uint64_t _a = (uint64_t)(a), _b = (uint64_t)(b);                                                               \
		if (_a == _b)                                                                                                  \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr,                                                                                            \
					"[FAIL] %s:%d: %s (%llu) != %s (%llu)\n",                                                          \
					__FILE__,                                                                                          \
					__LINE__,                                                                                          \
					#a,                                                                                                \
					(unsigned long long)_a,                                                                            \
					#b,                                                                                                \
					(unsigned long long)_b);                                                                           \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_EQ_FLOAT(a, b, eps)                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		double _a = (double)(a), _b = (double)(b);                                                                     \
		if (fabs(_a - _b) <= (eps))                                                                                    \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr,                                                                                            \
					"[FAIL] %s:%d: %s (%f) != %s (%f) [eps %f]\n",                                                     \
					__FILE__,                                                                                          \
					__LINE__,                                                                                          \
					#a,                                                                                                \
					_a,                                                                                                \
					#b,                                                                                                \
					_b,                                                                                                \
					(double)(eps));                                                                                    \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_EQ_CSTR(a, b)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		const char *_a = (a), *_b = (b);                                                                               \
		if (strcmp(_a, _b) == 0)                                                                                       \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr, "[FAIL] %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, _a, _b);                           \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_EQ_MEM(a, b, len)                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if (memcmp((a), (b), (len)) == 0)                                                                              \
		{                                                                                                              \
			global_test_stats.passed++;                                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			global_test_stats.failed++;                                                                                \
			fprintf(stderr, "[FAIL] %s:%d: memory mismatch over %zu bytes\n", __FILE__, __LINE__, (size_t)(len));      \
		}                                                                                                              \
	} while (0)

#define GLINT_TEST_CHECK_NULL(p) TEST_CHECK_MSG((p) == NULL, "%s expected NULL, got %p", #p, (void *)(p))
#define GLINT_TEST_CHECK_NOT_NULL(p) TEST_CHECK_MSG((p) != NULL, "%s expected non-NULL", #p)

// ---- sections / timing ----------------------------------------------------

#define GLINT_TEST_SECTION(name) fprintf(stderr, "\n-- %s --\n", name)

#define GLINT_TEST_TIMED(label, body)                                                                                  \
	do                                                                                                                 \
	{                                                                                                                  \
		struct timespec _t0, _t1;                                                                                      \
		clock_gettime(CLOCK_MONOTONIC, &_t0);                                                                          \
		body;                                                                                                          \
		clock_gettime(CLOCK_MONOTONIC, &_t1);                                                                          \
		double _ms = (_t1.tv_sec - _t0.tv_sec) * 1000.0 + (_t1.tv_nsec - _t0.tv_nsec) / 1e6;                           \
		fprintf(stderr, "  [%s] %.3f ms\n", label, _ms);                                                               \
	} while (0)

// ---- suite registration (optional, avoids editing main.c per test) -------

typedef void (*PFN_test_entry)(void);

typedef struct test_entry
{
	char const	  *name;
	PFN_test_entry fn;
} test_entry;

#define GLINT_TEST_MAX_ENTRIES 512

// Declare globals (defined in g_test_main.c)
extern test_entry global_test_entries[GLINT_TEST_MAX_ENTRIES];
extern int global_test_entry_count;

static inline void test_register(char const *name, PFN_test_entry fn)
{
	global_test_entries[global_test_entry_count].name = name;
	global_test_entries[global_test_entry_count].fn	  = fn;
	global_test_entry_count++;
}

#define GLINT_TEST_REGISTER(fn) test_register(#fn, fn)

static inline int test_run_all(char const *filter)
{
	for (int i = 0; i < global_test_entry_count; i++)
	{
		if (filter && !strstr(global_test_entries[i].name, filter))
		{
			continue;
		}
		int before_fail = global_test_stats.failed;
		fprintf(stderr, "\n== %s ==\n", global_test_entries[i].name);
		global_test_entries[i].fn();
		if (global_test_stats.failed == before_fail)
		{
			fprintf(stderr, "  OK\n");
		}
	}
	fprintf(stderr, "\n%d passed, %d failed\n", global_test_stats.passed, global_test_stats.failed);
	return global_test_stats.failed > 0 ? 1 : 0;
}

#endif // GLINT_TEST_COMMON_H
