#include "tests/g_test_common.h"
#include "base/base.h"

static void *lifecycle_proc(void *arg)
{
	(void)arg;
	return (void *)(uintptr_t)42;
}

static void test_os_thread_lifecycle(void)
{
	GLINT_TEST_SECTION("os_thread lifecycle");

	os_thread t = os_thread_create(string_lit("lifecycle"), lifecycle_proc, 0);
	GLINT_TEST_CHECK(t.opaque[0] != 0);
	GLINT_TEST_CHECK(os_thread_get_state(t) == os_thread_state_created);

	os_thread_start(t);
	GLINT_TEST_CHECK(os_thread_is_running(t));
	GLINT_TEST_CHECK(os_thread_get_state(t) == os_thread_state_running);

	void *ret = os_thread_join(t);
	GLINT_TEST_CHECK(ret == (void *)(uintptr_t)42);
	GLINT_TEST_CHECK(os_thread_get_state(t) == os_thread_state_joined);

	os_thread_destroy(t);
}

static void test_os_thread_self(void)
{
	GLINT_TEST_SECTION("os_thread attach_current / get_current / equal");

	// Create an entity for the main thread but never start it.
	os_thread self = os_thread_create(string_lit("main_self"), 0, 0);
	GLINT_TEST_CHECK(self.opaque[0] != 0);

	os_thread_attach_current(self);
	GLINT_TEST_CHECK(os_thread_get_state(self) == os_thread_state_running);

	os_thread cur = os_thread_get_current();
	GLINT_TEST_CHECK(cur.opaque[0] != 0);
	GLINT_TEST_CHECK(os_thread_equal(self, cur));

	// The name we gave it is reflected.
	string name = os_thread_get_name(cur);
	GLINT_TEST_CHECK(string_match(name, string_lit("main_self"), 0));

	os_thread_destroy(self);
}

static void *detach_proc(void *arg)
{
	(void)arg;
	return (void *)(uintptr_t)1;
}

static void test_os_thread_detach(void)
{
	GLINT_TEST_SECTION("os_thread detach");

	os_thread t = os_thread_create(string_lit("detacher"), detach_proc, 0);
	os_thread_start(t);
	GLINT_TEST_CHECK(os_thread_get_state(t) == os_thread_state_running);

	os_thread_detach(t);
	GLINT_TEST_CHECK(os_thread_get_state(t) == os_thread_state_detached);

	// Give the thread time to finish its minimal work, then destroy.
	os_thread_sleep(50);
	os_thread_destroy(t);
	GLINT_TEST_CHECK(1);
}

static void *exit_proc(void *arg)
{
	(void)arg;
	os_thread_exit((void *)(uintptr_t)99);
	return 0; // unreachable
}

static void test_os_thread_exit(void)
{
	GLINT_TEST_SECTION("os_thread exit");

	os_thread t = os_thread_create(string_lit("exiter"), exit_proc, 0);
	os_thread_start(t);
	void *ret = os_thread_join(t);
	GLINT_TEST_CHECK(ret == (void *)(uintptr_t)99);
	os_thread_destroy(t);
}

static void *sleep_proc(void *arg)
{
	(void)arg;
	os_thread_sleep(30);
	return 0;
}

static void test_os_thread_name_and_id(void)
{
	GLINT_TEST_SECTION("os_thread name / id");

	os_thread t = os_thread_create(string_lit("named_thread"), sleep_proc, 0);
	GLINT_TEST_CHECK(string_match(os_thread_get_name(t), string_lit("named_thread"), 0));

	// id is zero before start (opaque pthread_t is zero-initialised).
	GLINT_TEST_CHECK_EQ_U64(os_thread_get_id(t), 0);

	os_thread_start(t);
	GLINT_TEST_CHECK(os_thread_get_id(t) != 0);

	os_thread_join(t);
	os_thread_destroy(t);
}

static void test_os_thread_sleep_duration(void)
{
	GLINT_TEST_SECTION("os_thread sleep duration");

	os_time before = os_time_monotonic();
	os_thread_sleep(80);
	os_time after = os_time_monotonic();

	os_time delta = os_time_sub(after, before);
	u64		ms	  = os_time_as_ms(delta);
	GLINT_TEST_CHECK(ms >= 50); // generous lower bound for 80ms sleep
}

static void test_os_mutex_basic(void)
{
	GLINT_TEST_SECTION("os_mutex lock / try_lock");

	os_mutex m = os_mutex_create();
	GLINT_TEST_CHECK(m.opaque[0] != 0);

	os_mutex_lock(m);
	// try_lock from the owning thread must not deadlock; POSIX normal
	// mutex returns EBUSY → our wrapper returns 0 (false).
	GLINT_TEST_CHECK(!os_mutex_try_lock(m));
	os_mutex_unlock(m);

	// Now try_lock succeeds.
	GLINT_TEST_CHECK(os_mutex_try_lock(m));
	os_mutex_unlock(m);

	os_mutex_destroy(m);
}

typedef struct contended_state contended_state;
struct contended_state
{
	os_mutex m;
	u64		 counter;
	u64		 iters;
};

static void *contended_proc(void *arg)
{
	contended_state *s = (contended_state *)arg;
	for (u64 i = 0; i < s->iters; i++)
	{
		os_mutex_lock(s->m);
		s->counter++;
		os_mutex_unlock(s->m);
	}
	return 0;
}

static void test_os_mutex_contention(void)
{
	GLINT_TEST_SECTION("os_mutex contended counter");

	enum
	{
		N	  = 4,
		ITERS = 5000
	};

	arena			*a = arena_create_default(.name = "mutex_ct");
	contended_state *s = arena_push_struct(a, contended_state);
	s->m			   = os_mutex_create();
	s->counter		   = 0;
	s->iters		   = ITERS;

	os_thread threads[N];
	for (u64 i = 0; i < N; i++)
	{
		threads[i] = os_thread_create(string_lit("w"), contended_proc, s);
		os_thread_start(threads[i]);
	}
	for (u64 i = 0; i < N; i++)
		os_thread_join(threads[i]);
	for (u64 i = 0; i < N; i++)
		os_thread_destroy(threads[i]);

	os_mutex_destroy(s->m);
	GLINT_TEST_CHECK_EQ_U64(s->counter, N * ITERS);
	arena_destroy(a);
}

typedef struct condvar_signal_state condvar_signal_state;
struct condvar_signal_state
{
	os_mutex	m;
	os_cond_var cv;
	b8			data_ready;
	b8			waiter_parked;
	u64			data;
};

static void *condvar_waiter_proc(void *arg)
{
	condvar_signal_state *s = (condvar_signal_state *)arg;
	os_mutex_lock(s->m);
	s->waiter_parked = 1;
	while (!s->data_ready)
		os_cond_var_wait(s->cv, s->m);
	os_mutex_unlock(s->m);
	return 0;
}

static void test_os_cond_var_signal(void)
{
	GLINT_TEST_SECTION("os_cond_var signal");

	arena				 *a = arena_create_default(.name = "cv_sig");
	condvar_signal_state *s = arena_push_struct(a, condvar_signal_state);
	s->m					= os_mutex_create();
	s->cv					= os_cond_var_create();
	s->data_ready			= 0;
	s->waiter_parked		= 0;
	s->data					= 0;

	os_thread waiter = os_thread_create(string_lit("waiter"), condvar_waiter_proc, s);
	os_thread_start(waiter);

	{
		os_mutex_lock(s->m);
		while (!s->waiter_parked)
		{
			os_mutex_unlock(s->m);
			os_thread_sleep(1);
			os_mutex_lock(s->m);
		}
		s->data_ready = 1;
		s->data		  = 42;
		os_cond_var_signal(s->cv);
		os_mutex_unlock(s->m);
	}

	os_thread_join(waiter);
	os_thread_destroy(waiter);

	GLINT_TEST_CHECK_EQ_U64(s->data, 42);
	os_cond_var_destroy(s->cv);
	os_mutex_destroy(s->m);
	arena_destroy(a);
}

typedef struct condvar_bcast_state condvar_bcast_state;
struct condvar_bcast_state
{
	os_mutex	m;
	os_cond_var cv;
	b8			data_ready;
	b8			waiter_parked[2];
	u32			woke_count;
	u64			data;
};

typedef struct bcast_waiter_arg bcast_waiter_arg;
struct bcast_waiter_arg
{
	condvar_bcast_state *shared;
	u32					 slot;
};

static void *condvar_bcast_waiter_proc(void *arg)
{
	bcast_waiter_arg	*ta = (bcast_waiter_arg *)arg;
	condvar_bcast_state *s	= ta->shared;
	os_mutex_lock(s->m);
	s->waiter_parked[ta->slot] = 1;
	while (!s->data_ready)
	{
		os_cond_var_wait(s->cv, s->m);
	}
	s->woke_count++;
	os_mutex_unlock(s->m);
	return 0;
}

static void test_os_cond_var_broadcast(void)
{
	GLINT_TEST_SECTION("os_cond_var broadcast");

	arena				*a = arena_create_default(.name = "cv_bcast");
	condvar_bcast_state *s = arena_push_struct(a, condvar_bcast_state);
	g_memory_zero_struct(s); // must zero BEFORE creating the handles below
	s->m		  = os_mutex_create();
	s->cv		  = os_cond_var_create();
	s->data_ready = 0;
	s->woke_count = 0;

	bcast_waiter_arg a1 = {s, 0};
	bcast_waiter_arg a2 = {s, 1};
	os_thread		 w1 = os_thread_create(string_lit("w1"), condvar_bcast_waiter_proc, &a1);
	os_thread		 w2 = os_thread_create(string_lit("w2"), condvar_bcast_waiter_proc, &a2);
	os_thread_start(w1);
	os_thread_start(w2);

	{
		os_mutex_lock(s->m);
		while (!(s->waiter_parked[0] && s->waiter_parked[1]))
		{
			os_mutex_unlock(s->m);
			os_thread_sleep(1);
			os_mutex_lock(s->m);
		}
		s->data_ready = 1;
		os_cond_var_broadcast(s->cv);
		os_mutex_unlock(s->m);
	}

	os_thread_join(w1);
	os_thread_join(w2);
	os_thread_destroy(w1);
	os_thread_destroy(w2);

	GLINT_TEST_CHECK_EQ_INT((int)s->woke_count, 2);
	os_cond_var_destroy(s->cv);
	os_mutex_destroy(s->m);
	arena_destroy(a);
}

static void test_os_cond_var_timed_wait(void)
{
	GLINT_TEST_SECTION("os_cond_var timed_wait timeout");

	os_mutex	m  = os_mutex_create();
	os_cond_var cv = os_cond_var_create();

	os_mutex_lock(m);
	os_time before = os_time_monotonic();
	b8		result = os_cond_var_timed_wait(cv, m, 50);
	os_time after  = os_time_monotonic();
	os_mutex_unlock(m);

	GLINT_TEST_CHECK(!result); // timeout, no signal

	u64 elapsed_ms = os_time_as_ms(os_time_sub(after, before));
	GLINT_TEST_CHECK(elapsed_ms >= 30); // should be ~50ms, but allow jitter

	os_cond_var_destroy(cv);
	os_mutex_destroy(m);
}

static void test_os_semaphore_basic(void)
{
	GLINT_TEST_SECTION("os_semaphore try_wait / post / wait");

	os_semaphore s = os_semaphore_create(0, 1);
	GLINT_TEST_CHECK(s.opaque[0] != 0);

	// count 0 → try_wait fails.
	GLINT_TEST_CHECK(!os_semaphore_try_wait(s));

	os_semaphore_post(s);
	GLINT_TEST_CHECK(os_semaphore_try_wait(s));	 // count back to 0
	GLINT_TEST_CHECK(!os_semaphore_try_wait(s)); // exhausted

	os_semaphore_destroy(s);

	// Semaphore with plenty of tokens.
	os_semaphore s2 = os_semaphore_create(2, 10);
	os_semaphore_wait(s2);						 // count → 1
	os_semaphore_post(s2);						 // count → 2
	os_semaphore_post(s2);						 // count → 3
	os_semaphore_wait(s2);						 // count → 2
	GLINT_TEST_CHECK(os_semaphore_try_wait(s2)); // count → 1
	os_semaphore_destroy(s2);
}

typedef struct sem_sync_state sem_sync_state;
struct sem_sync_state
{
	os_semaphore s;
	b8			 worker_done;
};

static void *sem_worker_proc(void *arg)
{
	sem_sync_state *ss = (sem_sync_state *)arg;
	os_semaphore_wait(ss->s);
	ss->worker_done = 1;
	return 0;
}

static void test_os_semaphore_thread_sync(void)
{
	GLINT_TEST_SECTION("os_semaphore thread sync");

	arena		   *a  = arena_create_default(.name = "sem_sync");
	sem_sync_state *ss = arena_push_struct(a, sem_sync_state);
	ss->s			   = os_semaphore_create(0, 1);
	ss->worker_done	   = 0;

	os_thread worker = os_thread_create(string_lit("semw"), sem_worker_proc, ss);
	os_thread_start(worker);

	// Post wakes the worker; even if the worker hasn't entered wait yet,
	// the count tracks and wait succeeds immediately.
	os_semaphore_post(ss->s);

	os_thread_join(worker);
	os_thread_destroy(worker);

	GLINT_TEST_CHECK(ss->worker_done);
	os_semaphore_destroy(ss->s);
	arena_destroy(a);
}

typedef struct barrier_test_shared barrier_test_shared;
struct barrier_test_shared
{
	os_barrier b;
	os_mutex   m;
	u32		   arrived;	   // incremented before wait
	u32		   released;   // incremented after wait
	u32		   n;		   // participant count
	u8		   wait_ok[8]; // per-thread result (max 8 threads)
};

typedef struct barrier_thread_arg barrier_thread_arg;
struct barrier_thread_arg
{
	barrier_test_shared *shared;
	u32					 slot;
};

static void *barrier_participant_proc(void *arg)
{
	barrier_thread_arg	*ta = (barrier_thread_arg *)arg;
	barrier_test_shared *s	= ta->shared;

	os_mutex_lock(s->m);
	s->arrived++;
	os_mutex_unlock(s->m);

	b8 ok = os_barrier_wait(s->b);

	os_mutex_lock(s->m);
	s->released++;
	s->wait_ok[ta->slot] = (u8)ok;
	os_mutex_unlock(s->m);

	return 0;
}

static void test_os_barrier_multi_thread(void)
{
	GLINT_TEST_SECTION("os_barrier multi-thread");

	enum
	{
		N = 3
	};

	arena *a = arena_create_default(.name = "barrier");

	barrier_test_shared *s = arena_push_struct(a, barrier_test_shared);
	g_memory_zero_struct(s);
	s->b = os_barrier_create(N);
	s->m = os_mutex_create();
	s->n = N;

	barrier_thread_arg *args = arena_push_array(a, barrier_thread_arg, N);
	os_thread			threads[N];

	for (u32 i = 0; i < N; i++)
	{
		args[i].shared = s;
		args[i].slot   = i;
		threads[i]	   = os_thread_create(string_lit("bp"), barrier_participant_proc, &args[i]);
		os_thread_start(threads[i]);
	}

	for (u32 i = 0; i < N; i++)
		os_thread_join(threads[i]);
	for (u32 i = 0; i < N; i++)
		os_thread_destroy(threads[i]);

	GLINT_TEST_CHECK_EQ_INT((int)s->arrived, N);
	GLINT_TEST_CHECK_EQ_INT((int)s->released, N);

	// Every thread must report a successful barrier wait.
	for (u32 i = 0; i < N; i++)
	{
		GLINT_TEST_CHECK(s->wait_ok[i]);
	}

	os_barrier_destroy(s->b);
	os_mutex_destroy(s->m);
	arena_destroy(a);
}

static void test_os_barrier_single(void)
{
	GLINT_TEST_SECTION("os_barrier single-thread");

	os_barrier b = os_barrier_create(1);
	GLINT_TEST_CHECK(b.opaque[0] != 0);
	GLINT_TEST_CHECK(os_barrier_wait(b));
	os_barrier_destroy(b);
}

static void test_os_thread_yield(void)
{
	GLINT_TEST_SECTION("os_thread yield");

	// yield doesn't crash; that's the test.
	os_thread_yield();
	GLINT_TEST_CHECK(1);
}

static void register_os_threads_tests(void) __attribute__((constructor));
static void register_os_threads_tests(void)
{
	GLINT_TEST_REGISTER(test_os_thread_lifecycle);
	GLINT_TEST_REGISTER(test_os_thread_self);
	GLINT_TEST_REGISTER(test_os_thread_detach);
	GLINT_TEST_REGISTER(test_os_thread_exit);
	GLINT_TEST_REGISTER(test_os_thread_name_and_id);
	GLINT_TEST_REGISTER(test_os_thread_sleep_duration);
	GLINT_TEST_REGISTER(test_os_thread_yield);
	GLINT_TEST_REGISTER(test_os_mutex_basic);
	GLINT_TEST_REGISTER(test_os_mutex_contention);
	GLINT_TEST_REGISTER(test_os_cond_var_signal);
	GLINT_TEST_REGISTER(test_os_cond_var_broadcast);
	GLINT_TEST_REGISTER(test_os_cond_var_timed_wait);
	GLINT_TEST_REGISTER(test_os_semaphore_basic);
	GLINT_TEST_REGISTER(test_os_semaphore_thread_sync);
	GLINT_TEST_REGISTER(test_os_barrier_multi_thread);
	GLINT_TEST_REGISTER(test_os_barrier_single);
}

#include "base/base.c"
