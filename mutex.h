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

#ifndef CM_MUTEX_H
#define CM_MUTEX_H

#include "dll.h"
#include "types.h"
#include "header.h"

CM_BEGIN_EXTERN

typedef struct cmMutex {
#if defined(CM_SYS_WINDOWS)
	CRITICAL_SECTION win32_critical_section;
#else
	pthread_mutex_t pthread_mutex;
	pthread_mutexattr_t pthread_mutexattr;
#endif
} cmMutex;

CM_DEF void cm_mutex_init    (cmMutex *m);
CM_DEF void cm_mutex_destroy (cmMutex *m);
CM_DEF void cm_mutex_lock    (cmMutex *m);
CM_DEF b32  cm_mutex_try_lock(cmMutex *m);
CM_DEF void cm_mutex_unlock  (cmMutex *m);

// NOTE(bill): If you wanted a Scoped Mutex in C++, why not use the defer() construct?
// No need for a silly wrapper class and it's clear!
#if 0
cmMutex m = {0};
cm_mutex_init(&m);
{
	cm_mutex_lock(&m);
	defer (cm_mutex_unlock(&m));

	// Do whatever as the mutex is now scoped based!
}
#endif

CM_END_EXTERN

#endif //CM_MUTEX_H