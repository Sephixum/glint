#include "base/core/g_platform_and_compiler_defines.h"
#include "base/core/g_types.h"

#include "base/memory/g_arena.h"

#include "base/os/g_os_memory.h"
#include "base/os/g_os_system_info.h"
#include "base/os/g_os_thread.h"
#include "base/os/g_os_mutex.h"
#include "base/os/g_os_cond_var.h"
#include "base/os/g_os_semaphore.h"
#include "base/os/g_os_barrier.h"
#include "base/os/g_os_file.h"
#include "base/os/g_os_library.h"
#include "base/os/g_os_time.h"

#include "base/util/g_memory.h"
#include "base/util/g_linked_list.h"
#include "base/util/g_util.h"

#include <memory.h>
#include <bits/pthreadtypes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

typedef struct lnx_file
{
	int fd;
} lnx_file;

typedef struct lnx_barrier
{
	pthread_barrier_t *barrier;
} lnx_barrier;

typedef struct lnx_library
{
	void *handle;
} lnx_library;

typedef struct lnx_mutex
{
	pthread_mutex_t *mutex;
} lnx_mutex;

typedef struct lnx_semaphore
{
	sem_t *sem;
} lnx_semaphore;

typedef struct lnx_cond_var
{
	pthread_cond_t *cond;
} lnx_cond_var;

typedef struct lnx_thread
{
	pthread_t		   thread;
	PFN_os_thread_proc proc;
	void			  *arg;
	os_thread_state	   state;
	string			   name;
} lnx_thread;

typedef enum entity_kind
{
	entity_kind_file,
	entity_kind_barrier,
	entity_kind_library,
	entity_kind_mutex,
	entity_kind_semaphore,
	entity_kind_cond_var,
	entity_kind_thread,

} entity_kind;

typedef struct entity
{
	struct entity *next;
	entity_kind	   kind;
	union
	{
		lnx_file	  file;
		lnx_barrier	  barrier;
		lnx_library	  library;
		lnx_mutex	  mutex;
		lnx_semaphore semaphore;
		lnx_cond_var  cond_var;
		lnx_thread	  thread;
	};
} entity;

typedef struct lnx_state
{
	arena		   *a;
	arena		   *entity_a;
	pthread_mutex_t entity_mutex;
	entity		   *entity_free;
	pthread_key_t	tls_thread_key;
} lnx_state;

global lnx_state *linux_state = 0;

internal void ensure_state_exists(void)
{
	if (linux_state != 0)
	{
		return;
	}

	arena *main_state_a	  = arena_create_default();
	linux_state			  = arena_push_struct(main_state_a, lnx_state);
	linux_state->a		  = main_state_a;
	linux_state->entity_a = arena_create_default();
	pthread_mutex_init(&linux_state->entity_mutex, 0);

	entity *pool = arena_push_array(linux_state->entity_a, entity, 64);
	g_for_each_index(i, 63)
	{
		pool[i].next = &pool[i + 1];
	}
	pool[63].next			 = 0;
	linux_state->entity_free = pool;

	pthread_key_create(&linux_state->tls_thread_key, 0);
}

internal entity *entity_create(entity_kind kind)
{
	ensure_state_exists();

	entity *e = 0;
	g_defer_scope(pthread_mutex_lock(&linux_state->entity_mutex), pthread_mutex_unlock(&linux_state->entity_mutex))
	{
		e = linux_state->entity_free;
		if (e)
		{
			linux_state->entity_free = e->next;
		}
		else
		{
			e = arena_push_struct(linux_state->entity_a, entity);
		}
	}
	g_memory_zero_struct(e);
	e->kind = kind;
	return e;
}

internal void entity_destroy(entity *e)
{
	ensure_state_exists();

	g_defer_scope(pthread_mutex_lock(&linux_state->entity_mutex), pthread_mutex_unlock(&linux_state->entity_mutex))
	{
		g_single_ll_stack_push(linux_state->entity_free, e);
	}
}

// system_info
internal system_info *os_get_system_info(void)
{
	local_persist system_info info	 = {0};
	local_persist b8		  cached = 0;

	if (!cached)
	{
		info.logical_processor_count = (u32)sysconf(_SC_NPROCESSORS_ONLN);
		info.page_size				 = (u64)getpagesize();
		info.large_page_size		 = info.page_size;
		info.allocation_granularity	 = info.page_size;
		gethostname(info.machine_name, sizeof(info.machine_name));
		info.machine_name[sizeof(info.machine_name) - 1] = 0;
		cached											 = 1;
	}

	return &info;
}

// memory
internal void *os_memory_reserve(u64 size)
{
	if (size == 0)
	{
		return 0;
	}

	void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}

	return result;
}

internal b8 os_memory_commit(void *ptr, u64 size)
{
	i32 res = 0;

	res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	if (res < 0)
	{
		return 0;
	}

	res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
	if (res < 0)
	{
		return 0;
	}

	return 1;
}

internal void os_memory_decommit(void *ptr, u64 size)
{
	madvise(ptr, size, MADV_DONTNEED);
	mprotect(ptr, size, PROT_NONE);
}

internal void os_memory_release(void *ptr, u64 size)
{
	munmap(ptr, size);
}

internal void *os_memory_reserve_large(u64 size)
{
	void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}
	return result;
}

internal b32 os_memory_commit_large(void *ptr, u64 size)
{
	i32 res = 0;

	res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	if (res < 0)
	{
		return 0;
	}

	res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
	if (res < 0)
	{
		return 0;
	}

	return 1;
}

// thread

internal void *_thread_start_wrapper(void *arg)
{
	entity *e = (entity *)arg;
	pthread_setspecific(linux_state->tls_thread_key, e);
	return e->thread.proc(e->thread.arg);
}

internal os_thread os_thread_create(string name, PFN_os_thread_proc proc, void *user)
{
	entity *e		 = entity_create(entity_kind_thread);
	e->thread.proc	 = proc;
	e->thread.arg	 = user;
	e->thread.state	 = os_thread_state_created;
	e->thread.name	 = name;
	e->thread.thread = (pthread_t){0};
	return (os_thread){.opaque = {e}};
}

internal void os_thread_start(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	if (e->thread.state != os_thread_state_created)
	{
		return;
	}
	pthread_create(&e->thread.thread, 0, _thread_start_wrapper, e);
	e->thread.state = os_thread_state_running;
}

internal void *os_thread_join(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	if (e->thread.state != os_thread_state_running)
	{
		return 0;
	}
	void *retval = 0;
	pthread_join(e->thread.thread, &retval);
	e->thread.state = os_thread_state_joined;
	return retval;
}

internal void os_thread_detach(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	if (e->thread.state != os_thread_state_running)
	{
		return;
	}
	pthread_detach(e->thread.thread);
	e->thread.state = os_thread_state_detached;
}

internal void os_thread_destroy(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	entity_destroy(e);
}

internal void os_thread_attach_current(os_thread t)
{
	ensure_state_exists();
	entity *e		 = (entity *)t.opaque[0];
	e->kind			 = entity_kind_thread;
	e->thread.thread = pthread_self();
	e->thread.state	 = os_thread_state_running;
	if (e->thread.name.size == 0)
	{
		e->thread.name = s("main");
	}
	pthread_setspecific(linux_state->tls_thread_key, e);
}

internal os_thread os_thread_get_current(void)
{
	ensure_state_exists();
	entity *e = pthread_getspecific(linux_state->tls_thread_key);
	return (os_thread){.opaque = {e}};
}

internal b8 os_thread_equal(os_thread a, os_thread b)
{
	entity *ea = (entity *)a.opaque[0];
	entity *eb = (entity *)b.opaque[0];
	return (b8)pthread_equal(ea->thread.thread, eb->thread.thread);
}

internal os_thread_state os_thread_get_state(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	return e->thread.state;
}

internal b8 os_thread_is_running(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	return e->thread.state == os_thread_state_running;
}

internal string os_thread_get_name(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	return e->thread.name;
}

internal u64 os_thread_get_id(os_thread t)
{
	entity *e = (entity *)t.opaque[0];
	return (u64)e->thread.thread;
}

internal void os_thread_sleep(u32 milliseconds)
{
	struct timespec ts = {
		.tv_sec	 = milliseconds / 1'000,
		.tv_nsec = (milliseconds % 1'000) * 1'000'000,
	};
	nanosleep(&ts, 0);
}

internal void os_thread_yield(void)
{
	sched_yield();
}

internal void os_thread_exit(void *retval)
{
	pthread_exit(retval);
}

// mutex

internal os_mutex os_mutex_create(void)
{
	entity *e	   = entity_create(entity_kind_mutex);
	e->mutex.mutex = arena_push_struct(linux_state->entity_a, pthread_mutex_t);
	pthread_mutex_init(e->mutex.mutex, 0);
	return (os_mutex){.opaque = {e}};
}

internal void os_mutex_destroy(os_mutex m)
{
	entity *e = (entity *)m.opaque[0];
	pthread_mutex_destroy(e->mutex.mutex);
	entity_destroy(e);
}

internal void os_mutex_lock(os_mutex m)
{
	entity *e = (entity *)m.opaque[0];
	pthread_mutex_lock(e->mutex.mutex);
}

internal b8 os_mutex_try_lock(os_mutex m)
{
	entity *e = (entity *)m.opaque[0];
	return pthread_mutex_trylock(e->mutex.mutex) == 0;
}

internal void os_mutex_unlock(os_mutex m)
{
	entity *e = (entity *)m.opaque[0];
	pthread_mutex_unlock(e->mutex.mutex);
}

// cond_var

internal os_cond_var os_cond_var_create(void)
{
	entity *e		 = entity_create(entity_kind_cond_var);
	e->cond_var.cond = arena_push_struct(linux_state->entity_a, pthread_cond_t);
	pthread_cond_init(e->cond_var.cond, 0);
	return (os_cond_var){.opaque = {e}};
}

internal void os_cond_var_destroy(os_cond_var cv)
{
	entity *e = (entity *)cv.opaque[0];
	pthread_cond_destroy(e->cond_var.cond);
	entity_destroy(e);
}

internal void os_cond_var_wait(os_cond_var cv, os_mutex m)
{
	entity *e  = (entity *)cv.opaque[0];
	entity *em = (entity *)m.opaque[0];
	pthread_cond_wait(e->cond_var.cond, em->mutex.mutex);
}

internal b8 os_cond_var_timed_wait(os_cond_var cv, os_mutex m, u64 timeout_ms)
{
	entity *e  = (entity *)cv.opaque[0];
	entity *em = (entity *)m.opaque[0];

	struct timespec ts = {0};
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += timeout_ms / 1'000;
	ts.tv_nsec += (timeout_ms % 1'000) * 1'000'000;
	if (ts.tv_nsec >= 1'000'000'000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1'000'000'000;
	}
	return pthread_cond_timedwait(e->cond_var.cond, em->mutex.mutex, &ts) == 0;
}

internal void os_cond_var_signal(os_cond_var cv)
{
	entity *e = (entity *)cv.opaque[0];
	pthread_cond_signal(e->cond_var.cond);
}

internal void os_cond_var_broadcast(os_cond_var cv)
{
	entity *e = (entity *)cv.opaque[0];
	pthread_cond_broadcast(e->cond_var.cond);
}

// semaphore
internal os_semaphore os_semaphore_create(u32 initial_count, u32 max_count)
{
	// POSIX unnamed semaphores have no max; initial is enough
	(void)max_count;
	entity *e		 = entity_create(entity_kind_semaphore);
	e->semaphore.sem = arena_push_struct(linux_state->entity_a, sem_t);
	sem_init(e->semaphore.sem, 0, initial_count);
	return (os_semaphore){.opaque = {e}};
}

internal void os_semaphore_destroy(os_semaphore s)
{
	entity *e = (entity *)s.opaque[0];
	sem_destroy(e->semaphore.sem);
	entity_destroy(e);
}

internal void os_semaphore_wait(os_semaphore s)
{
	entity *e = (entity *)s.opaque[0];
	sem_wait(e->semaphore.sem);
}

internal b8 os_semaphore_try_wait(os_semaphore s)
{
	entity *e = (entity *)s.opaque[0];
	return sem_trywait(e->semaphore.sem) == 0;
}

internal void os_semaphore_post(os_semaphore s)
{
	entity *e = (entity *)s.opaque[0];
	sem_post(e->semaphore.sem);
}

// barrier
internal os_barrier os_barrier_create(u32 thread_count)
{
	entity *e		   = entity_create(entity_kind_barrier);
	e->barrier.barrier = arena_push_struct(linux_state->entity_a, pthread_barrier_t);
	pthread_barrier_init(e->barrier.barrier, 0, thread_count);
	return (os_barrier){.opaque = {e}};
}

internal void os_barrier_destroy(os_barrier b)
{
	entity *e = (entity *)b.opaque[0];
	pthread_barrier_destroy(e->barrier.barrier);
	entity_destroy(e);
}

internal b8 os_barrier_wait(os_barrier b)
{
	entity *e = (entity *)b.opaque[0];
	int		r = pthread_barrier_wait(e->barrier.barrier);
	return r == 0 || r == PTHREAD_BARRIER_SERIAL_THREAD;
}

// file

internal os_file os_file_open(string path, os_file_mode mode)
{
	int flags = 0;
	if (mode & os_file_mode_read && mode & os_file_mode_write)
	{
		flags = O_RDWR | O_CREAT;
	}
	else if (mode & os_file_mode_write)
	{
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	}
	else
	{
		flags = O_RDONLY;
	}

	if (mode & os_file_mode_append)
	{
		flags |= O_APPEND;
	}

	// Build a null-terminated cstr from the string
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	int fd = open((char *)buf, flags, 0644);
	if (fd < 0)
	{
		return (os_file){.opaque = {0}};
	}

	entity *e  = entity_create(entity_kind_file);
	e->file.fd = fd;
	return (os_file){.opaque = {e}};
}

internal void os_file_close(os_file f)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return;
	}
	close(e->file.fd);
	entity_destroy(e);
}

internal u64 os_file_write(os_file f, void const *data, u64 data_size)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return 0;
	}
	return (u64)write(e->file.fd, data, data_size);
}

internal string os_file_read(arena *a, os_file f, u64 max_read)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return string_zero();
	}

	u8 *buf		   = (u8 *)arena_push(a, max_read, 1, 0);
	i64 bytes_read = read(e->file.fd, buf, max_read);
	if (bytes_read < 0)
	{
		return string_zero();
	}
	return string_create(buf, (u64)bytes_read);
}

internal string os_file_read_entire(arena *a, os_file f)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return string_zero();
	}

	struct stat st;
	if (fstat(e->file.fd, &st) < 0)
	{
		return string_zero();
	}

	u64 size = (u64)st.st_size;
	u8 *buf	 = (u8 *)arena_push(a, size, 1, 0);

	lseek(e->file.fd, 0, SEEK_SET);
	i64 bytes_read = read(e->file.fd, buf, size);
	if (bytes_read < 0 || (u64)bytes_read < size)
	{
		return string_zero();
	}

	return string_create(buf, size);
}

internal b8 os_file_flush(os_file f)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return 0;
	}
	return fsync(e->file.fd) == 0;
}

internal b8 os_file_valid(os_file f)
{
	entity *e = (entity *)f.opaque[0];
	return e != 0 && fcntl(e->file.fd, F_GETFD) != -1;
}

internal u64 os_file_size(os_file f)
{
	entity *e = (entity *)f.opaque[0];
	if (!e)
	{
		return 0;
	}

	struct stat st = {0};
	if (fstat(e->file.fd, &st) < 0)
	{
		return 0;
	}
	return (u64)st.st_size;
}

// symlink

internal b8 os_symlink_create(string target, string path)
{
	u8	target_buf[1024];
	u64 tgt_copy = target.size < sizeof(target_buf) - 1 ? target.size : sizeof(target_buf) - 1;
	g_memory_copy(target_buf, target.str, tgt_copy);
	target_buf[tgt_copy] = 0;

	u8	path_buf[1024];
	u64 pth_copy = path.size < sizeof(path_buf) - 1 ? path.size : sizeof(path_buf) - 1;
	g_memory_copy(path_buf, path.str, pth_copy);
	path_buf[pth_copy] = 0;

	return symlink((char *)target_buf, (char *)path_buf) == 0;
}

internal b8 os_symlink_remove(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	return unlink((char *)buf) == 0;
}

internal b8 os_symlink_remove_recursive(string path)
{
	return os_symlink_remove(path);
}

// directory

internal b8 os_dir_create(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	return mkdir((char *)buf, 0755) == 0;
}

internal b8 os_dir_create_recursive(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	// Walk forward, mkdir each component
	for (char *p = (char *)buf + 1; *p; ++p)
	{
		if (*p == '/')
		{
			*p = 0;
			mkdir((char *)buf, 0755);
			*p = '/';
		}
	}
	return mkdir((char *)buf, 0755) == 0;
}

internal b8 os_dir_remove(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	return rmdir((char *)buf) == 0;
}

internal b8 os_dir_remove_recursive(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	DIR *dir = opendir((char *)buf);
	if (!dir)
	{
		return 0;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != 0)
	{
		if (entry->d_name[0] == '.' && (entry->d_name[1] == 0 || (entry->d_name[1] == '.' && entry->d_name[2] == 0)))
		{
			continue;
		}

		u8	entry_buf[1024];
		int n = snprintf((char *)entry_buf, sizeof(entry_buf), "%s/%s", (char *)buf, entry->d_name);
		if (n < 0 || (u64)n >= sizeof(entry_buf))
			continue;

		struct stat st;
		if (lstat((char *)entry_buf, &st) < 0)
		{
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			os_dir_remove_recursive(string_from_cstr((char *)entry_buf));
		}
		else
		{
			unlink((char *)entry_buf);
		}
	}
	closedir(dir);

	return rmdir((char *)buf) == 0;
}

internal string_list os_dir_list(arena *a, string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	string_list list = {0};
	DIR		   *dir	 = opendir((char *)buf);
	if (!dir)
	{
		return list;
	}

	struct dirent *entry = 0;
	while ((entry = readdir(dir)) != 0)
	{
		if (entry->d_name[0] == '.' && (entry->d_name[1] == 0 || (entry->d_name[1] == '.' && entry->d_name[2] == 0)))
		{
			continue;
		}
		string_list_push(a, &list, string_from_cstr(entry->d_name));
	}
	closedir(dir);

	return list;
}

// path

internal os_path_type os_path_get_type(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	struct stat st = {0};
	if (lstat((char *)buf, &st) < 0)
	{
		return os_path_type_none;
	}

	if (S_ISREG(st.st_mode))
	{
		return os_path_type_file;
	}
	if (S_ISDIR(st.st_mode))
	{
		return os_path_type_dir;
	}
	if (S_ISLNK(st.st_mode))
	{
		return os_path_type_symlink;
	}
	return os_path_type_other;
}

internal os_path_info os_path_get_info(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	os_path_info info = {0};
	struct stat	 st;
	if (stat((char *)buf, &st) < 0)
	{
		return info; // exists = false
	}

	info.exists		   = 1;
	info.is_dir		   = (b8)S_ISDIR(st.st_mode);
	info.is_symlink	   = (b8)S_ISLNK(st.st_mode);
	info.size_bytes	   = (u64)st.st_size;
	info.modified_time = (os_time){
		.seconds	 = (u64)st.st_mtim.tv_sec,
		.nanoseconds = (u64)st.st_mtim.tv_nsec,
	};
	return info;
}

// time

internal os_time os_time_now(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (os_time){.seconds = (u64)ts.tv_sec, .nanoseconds = (u64)ts.tv_nsec};
}

internal os_time os_time_monotonic(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (os_time){.seconds = (u64)ts.tv_sec, .nanoseconds = (u64)ts.tv_nsec};
}

internal os_time os_time_sub(os_time a, os_time b)
{
	if (a.seconds < b.seconds || (a.seconds == b.seconds && a.nanoseconds < b.nanoseconds))
		return (os_time){0};

	u64 sec = a.seconds - b.seconds;
	if (a.nanoseconds >= b.nanoseconds)
		return (os_time){.seconds = sec, .nanoseconds = a.nanoseconds - b.nanoseconds};

	// borrow from seconds
	return (os_time){.seconds = sec - 1, .nanoseconds = 1'000'000'000 + a.nanoseconds - b.nanoseconds};
}

internal u64 os_time_as_ms(os_time t)
{
	return t.seconds * 1000 + t.nanoseconds / 1'000'000;
}

internal u64 os_time_as_us(os_time t)
{
	return t.seconds * 1'000'000 + t.nanoseconds / 1'000;
}

// Fill an os_date from a libc struct tm. The caller supplies utc_offset_seconds
// (0 for gmtime, tm_gmtoff for localtime).
internal os_date os_date_from_tm(struct tm *t, os_time inst, i32 utc_offset)
{
	os_date d;
	d.year				 = (u16)(t->tm_year + 1900);
	d.month				 = (u8)(t->tm_mon + 1);
	d.day				 = (u8)t->tm_mday;
	d.hour				 = (u8)t->tm_hour;
	d.minute			 = (u8)t->tm_min;
	d.second			 = (u8)t->tm_sec;
	d.nanosecond		 = inst.nanoseconds;
	d.weekday			 = (os_weekday)t->tm_wday; // Sunday == 0 on both libc
	d.day_of_year		 = (u16)(t->tm_yday + 1);
	d.is_dst			 = (b8)(t->tm_isdst > 0);
	d.utc_offset_seconds = utc_offset;
	return d;
}

internal os_date os_time_to_date(os_time t)
{
	time_t	  sec = (time_t)t.seconds;
	struct tm tm;
	localtime_r(&sec, &tm);
	return os_date_from_tm(&tm, t, (i32)tm.tm_gmtoff);
}

internal os_date os_time_to_date_utc(os_time t)
{
	time_t	  sec = (time_t)t.seconds;
	struct tm tm;
	gmtime_r(&sec, &tm);
	return os_date_from_tm(&tm, t, 0);
}

// library

internal os_library os_library_load(string path)
{
	u8	buf[1024];
	u64 copy_size = path.size < sizeof(buf) - 1 ? path.size : sizeof(buf) - 1;
	g_memory_copy(buf, path.str, copy_size);
	buf[copy_size] = 0;

	void *handle = dlopen((char *)buf, RTLD_NOW | RTLD_LOCAL);
	if (!handle)
	{
		return (os_library){.opaque = {0}};
	}
	entity *e		  = entity_create(entity_kind_library);
	e->library.handle = handle;
	return (os_library){.opaque = {e}};
}

internal void *os_library_symbol(os_library lib, string name)
{
	entity *e = (entity *)lib.opaque[0];
	if (!e)
	{
		return 0;
	}
	u8	buf[256];
	u64 copy_size = name.size < sizeof(buf) - 1 ? name.size : sizeof(buf) - 1;
	g_memory_copy(buf, name.str, copy_size);
	buf[copy_size] = 0;

	return dlsym(e->library.handle, (char *)buf);
}

internal void os_library_unload(os_library lib)
{
	entity *e = (entity *)lib.opaque[0];
	if (!e)
		return;
	dlclose(e->library.handle);
	entity_destroy(e);
}

internal string os_library_error(void)
{
	char *err = dlerror();
	return err ? string_from_cstr(err) : string_zero();
}

internal string os_file_error(void)
{
	return string_from_cstr(strerror(errno));
}
