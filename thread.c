/********************************************************************************
 * MIT License
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Copyright (c) 2020 Sarayu Nhookeaw
 *
 ********************************************************************************/

#include "thread.h"
#include "utils.h"
#include "assert.h"
#include "debug.h"
#include "header.h"
#include "memory.h"

CM_BEGIN_EXTERN

void 
cm_thread_init(cmThread *t) {
	cm_zero_item(t);
#if defined(CM_SYS_WINDOWS)
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	t->posix_handle = 0;
#endif
	cm_semaphore_init(&t->semaphore);
}

void 
cm_thread_destroy(cmThread *t) {
	if (t->is_running) cm_thread_join(t);
	cm_semaphore_destroy(&t->semaphore);
}

cm_inline void 
cm__thread_run(cmThread *t) {
	cm_semaphore_release(&t->semaphore);
	t->return_value = t->proc(t);
}

#if defined(CM_SYS_WINDOWS)
	cm_inline DWORD __stdcall cm__thread_proc(void *arg) {
		cmThread *t = cast(cmThread *)arg;
		cm__thread_run(t);
		t->is_running = false;
		return 0;
	}
#else
	cm_inline void *          cm__thread_proc(void *arg) {
		cmThread *t = cast(cmThread *)arg;
		cm__thread_run(t);
		t->is_running = false;
		return NULL;
	}
#endif

cm_inline void 
cm_thread_start(cmThread *t, cmThreadProc *proc, void *user_data) { 
    cm_thread_start_with_stack(t, proc, user_data, 0); 
}

cm_inline void 
cm_thread_start_with_stack(cmThread *t, cmThreadProc *proc, void *user_data, isize stack_size) {
	CM_ASSERT(!t->is_running);
	CM_ASSERT(proc != NULL);
	t->proc = proc;
	t->user_data = user_data;
	t->stack_size = stack_size;
	t->is_running = true;

#if defined(CM_SYS_WINDOWS)
	t->win32_handle = CreateThread(NULL, stack_size, cm__thread_proc, t, 0, NULL);
	CM_ASSERT_MSG(t->win32_handle != NULL, "CreateThread: GetLastError");
#else
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		if (stack_size != 0) {
			pthread_attr_setstacksize(&attr, stack_size);
		}
		pthread_create(&t->posix_handle, &attr, cm__thread_proc, t);
		pthread_attr_destroy(&attr);
	}
#endif

	cm_semaphore_wait(&t->semaphore);
}

cm_inline void 
cm_thread_join(cmThread *t) {
	if (!t->is_running) return;

#if defined(CM_SYS_WINDOWS)
	WaitForSingleObject(t->win32_handle, INFINITE);
	CloseHandle(t->win32_handle);
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	pthread_join(t->posix_handle, NULL);
	t->posix_handle = 0;
#endif
	t->is_running = false;
}

cm_inline b32 
cm_thread_is_running(cmThread const *t) { return t->is_running != 0; }

cm_inline u32 
cm_thread_current_id(void) {
	u32 thread_id;
#if defined(CM_SYS_WINDOWS)
	#if defined(CM_ARCH_32) && defined(CM_CPU_X86)
		thread_id = (cast(u32 *)__readfsdword(24))[9];
	#elif defined(CM_ARCH_64) && defined(CM_CPU_X86)
		thread_id = (cast(u32 *)__readgsqword(48))[18];
	#else
		thread_id = GetCurrentThreadId();
	#endif

#elif defined(CM_SYS_OSX) && defined(CM_ARCH_64)
	thread_id = pthread_mach_thread_np(pthread_self());
#elif defined(CM_ARCH_32) && defined(CM_CPU_X86)
	__asm__("mov %%gs:0x08,%0" : "=r"(thread_id));
#elif defined(CM_ARCH_64) && defined(CM_CPU_X86)
	__asm__("mov %%fs:0x10,%0" : "=r"(thread_id));
#else
	#error Unsupported architecture for cm_thread_current_id()
#endif

	return thread_id;
}

void 
cm_thread_set_name(cmThread *t, char const *name) {
#if defined(CM_COMPILER_MSVC)
	#pragma pack(push, 8)
		typedef struct {
			DWORD       type;
			char const *name;
			DWORD       id;
			DWORD       flags;
		} gbprivThreadName;
	#pragma pack(pop)
		gbprivThreadName tn;
		tn.type  = 0x1000;
		tn.name  = name;
		tn.id    = GetThreadId(cast(HANDLE)t->win32_handle);
		tn.flags = 0;

		__try {
			RaiseException(0x406d1388, 0, cm_size_of(tn)/4, cast(ULONG_PTR *)&tn);
		} __except(1 /*EXCEPTION_EXECUTE_HANDLER*/) {
		}

#elif defined(CM_SYS_WINDOWS) && !defined(CM_COMPILER_MSVC)
	// IMPORTANT TODO(bill): Set thread name for GCC/Clang on windows
	return;
#elif defined(CM_SYS_OSX)
	// TODO(bill): Test if this works
	pthread_setname_np(name);
#else
	// TODO(bill): Test if this works
	pthread_setname_np(t->posix_handle, name);
#endif
}

void 
cm_sync_init(cmSync *s) {
	cm_zero_item(s);
	cm_mutex_init(&s->mutex);
	cm_mutex_init(&s->start);
	cm_semaphore_init(&s->release);
}

void 
cm_sync_destroy(cmSync *s) {
	if (s->waiting)
		CM_PANIC("Cannot destroy while threads are waiting!");

	cm_mutex_destroy(&s->mutex);
	cm_mutex_destroy(&s->start);
	cm_semaphore_destroy(&s->release);
}

void 
cm_sync_set_target(cmSync *s, i32 count) {
	cm_mutex_lock(&s->start);

	cm_mutex_lock(&s->mutex);
	CM_ASSERT(s->target == 0);
	s->target = count;
	s->current = 0;
	s->waiting = 0;
	cm_mutex_unlock(&s->mutex);
}

void 
cm_sync_release(cmSync *s) {
	if (s->waiting) {
		cm_semaphore_release(&s->release);
	} else {
		s->target = 0;
		cm_mutex_unlock(&s->start);
	}
}

i32 
cm_sync_reach(cmSync *s) {
	i32 n;
	cm_mutex_lock(&s->mutex);
	CM_ASSERT(s->current < s->target);
	n = ++s->current; // NOTE(bill): Record this value to avoid possible race if `return s->current` was done
	if (s->current == s->target)
		cm_sync_release(s);
	cm_mutex_unlock(&s->mutex);
	return n;
}

void 
cm_sync_reach_and_wait(cmSync *s) {
	cm_mutex_lock(&s->mutex);
	CM_ASSERT(s->current < s->target);
	s->current++;
	if (s->current == s->target) {
		cm_sync_release(s);
		cm_mutex_unlock(&s->mutex);
	} else {
		s->waiting++;                   // NOTE(bill): Waiting, so one more waiter
		cm_mutex_unlock(&s->mutex);     // NOTE(bill): Release the mutex to other threads

		cm_semaphore_wait(&s->release); // NOTE(bill): Wait for merge completion

		cm_mutex_lock(&s->mutex);       // NOTE(bill): On merge completion, lock mutex
		s->waiting--;                   // NOTE(bill): Done waiting
		cm_sync_release(s);             // NOTE(bill): Restart the next waiter
		cm_mutex_unlock(&s->mutex);
	}
}

CM_END_EXTERN

