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

#include "mutex.h"
#include "utils.h"
#include "header.h"

CM_BEGIN_EXTERN

cm_inline void 
cm_mutex_init(cmMutex *m) {
#if defined(CM_SYS_WINDOWS)
	InitializeCriticalSection(&m->win32_critical_section);
#else
	pthread_mutexattr_init(&m->pthread_mutexattr);
	pthread_mutexattr_settype(&m->pthread_mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m->pthread_mutex, &m->pthread_mutexattr);
#endif
}

cm_inline void 
cm_mutex_destroy(cmMutex *m) {
#if defined(CM_SYS_WINDOWS)
	DeleteCriticalSection(&m->win32_critical_section);
#else
	pthread_mutex_destroy(&m->pthread_mutex);
#endif
}

cm_inline void 
cm_mutex_lock(cmMutex *m) {
#if defined(CM_SYS_WINDOWS)
	EnterCriticalSection(&m->win32_critical_section);
#else
	pthread_mutex_lock(&m->pthread_mutex);
#endif
}

cm_inline b32 
cm_mutex_try_lock(cmMutex *m) {
#if defined(CM_SYS_WINDOWS)
	return TryEnterCriticalSection(&m->win32_critical_section) != 0;
#else
	return pthread_mutex_trylock(&m->pthread_mutex) == 0;
#endif
}

cm_inline void 
cm_mutex_unlock(cmMutex *m) {
#if defined(CM_SYS_WINDOWS)
	LeaveCriticalSection(&m->win32_critical_section);
#else
	pthread_mutex_unlock(&m->pthread_mutex);
#endif
}

CM_END_EXTERN

