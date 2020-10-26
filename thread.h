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

#ifndef CM_THREAD_H
#define CM_THREAD_H

#include "dll.h"
#include "arch.h"
#include "types.h"
#include "semaphore.h"
#include "mutex.h"
#include "pragma.h"

CM_BEGIN_EXTERN

#define CM_THREAD_PROC(name) isize name(struct cmThread *thread)
typedef CM_THREAD_PROC(cmThreadProc);

typedef struct cmThread {
#if defined(CM_SYS_WINDOWS)
	void *        win32_handle;
#else
	pthread_t     posix_handle;
#endif

	cmThreadProc * proc;
	void *         user_data;
	isize          user_index;
	isize volatile return_value;

	cmSemaphore   semaphore;
	isize         stack_size;
	b32 volatile  is_running;
} cmThread;

CM_DEF void cm_thread_init            (cmThread *t);
CM_DEF void cm_thread_destroy         (cmThread *t);
CM_DEF void cm_thread_start           (cmThread *t, cmThreadProc *proc, void *data);
CM_DEF void cm_thread_start_with_stack(cmThread *t, cmThreadProc *proc, void *data, isize stack_size);
CM_DEF void cm_thread_join            (cmThread *t);
CM_DEF b32  cm_thread_is_running      (cmThread const *t);
CM_DEF u32  cm_thread_current_id      (void);
CM_DEF void cm_thread_set_name        (cmThread *t, char const *name);


// NOTE(bill): Thread Merge Operation
// Based on Sean Barrett's stb_sync
typedef struct cmSync {
	i32 target;  // Target Number of threads
	i32 current; // Threads to hit
	i32 waiting; // Threads waiting

	cmMutex start;
	cmMutex mutex;
	cmSemaphore release;
} cmSync;

CM_DEF void cm_sync_init          (cmSync *s);
CM_DEF void cm_sync_destroy       (cmSync *s);
CM_DEF void cm_sync_set_target    (cmSync *s, i32 count);
CM_DEF void cm_sync_release       (cmSync *s);
CM_DEF i32  cm_sync_reach         (cmSync *s);
CM_DEF void cm_sync_reach_and_wait(cmSync *s);


CM_END_EXTERN

#endif //CM_THREAD_H