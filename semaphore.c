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

#include "semaphore.h"
#include "utils.h"
#include "header.h"

CM_BEGIN_EXTERN

cm_inline void 
cm_semaphore_release(cmSemaphore *s) { cm_semaphore_post(s, 1); }

#if defined(CM_SYS_WINDOWS)
	cm_inline void 
    cm_semaphore_init(cmSemaphore *s) {
		s->win32_handle = CreateSemaphoreA(NULL, 0, I32_MAX, NULL);
	}

	cm_inline void 
    cm_semaphore_destroy(cmSemaphore *s) {
		CloseHandle(s->win32_handle);
	}
    
	cm_inline void 
    cm_semaphore_post(cmSemaphore *s, i32 count) {
		ReleaseSemaphore(s->win32_handle, count, NULL);
	}

	cm_inline void 
    cm_semaphore_wait(cmSemaphore *s) {
		WaitForSingleObjectEx(s->win32_handle, INFINITE, FALSE);
	}

#elif defined(CM_SYS_OSX)
	cm_inline void 
    cm_semaphore_init(cmSemaphore *s){ 
        semaphore_create(mach_task_self(), &s->osx_handle, SYNC_POLICY_FIFO, 0); 
    }

	cm_inline void 
    cm_semaphore_destroy(cmSemaphore *s) { 
        semaphore_destroy(mach_task_self(), s->osx_handle); 
    }

	cm_inline void 
    cm_semaphore_post(cmSemaphore *s, i32 count) { 
        while (count --> 0) semaphore_signal(s->osx_handle); 
    }

	cm_inline void 
    cm_semaphore_wait(cmSemaphore *s) { 
        semaphore_wait(s->osx_handle); 
    }

#elif defined(CM_SYS_UNIX)
	cm_inline void 
    cm_semaphore_init(cmSemaphore *s) { 
        sem_init(&s->unix_handle, 0, 0); 
    }

	cm_inline void 
    cm_semaphore_destroy(cmSemaphore *s){ 
        sem_destroy(&s->unix_handle); 
    }

	cm_inline void 
    cm_semaphore_post(cmSemaphore *s, i32 count) { 
        while (count --> 0) sem_post(&s->unix_handle); 
    }

	cm_inline void 
    cm_semaphore_wait(cmSemaphore *s){ 
        int i; 
        do { i = sem_wait(&s->unix_handle); } 
        while (i == -1 && errno == EINTR); 
    }

#else
#error
#endif

CM_END_EXTERN