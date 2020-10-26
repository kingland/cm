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

#ifndef CM_SEMAPHORE_H
#define CM_SEMAPHORE_H

#include "dll.h"
#include "arch.h"
#include "types.h"

CM_BEGIN_EXTERN

#if defined(CM_SYS_WINDOWS)
typedef struct cmSemaphore { void *win32_handle;}      cmSemaphore;
#elif defined(CM_SYS_OSX)
typedef struct cmSemaphore { semaphore_t osx_handle; } cmSemaphore;
#elif defined(CM_SYS_UNIX)
typedef struct cmSemaphore { sem_t unix_handle; }      cmSemaphore;
#endif

CM_DEF void cm_semaphore_init   (cmSemaphore *s);
CM_DEF void cm_semaphore_destroy(cmSemaphore *s);
CM_DEF void cm_semaphore_post   (cmSemaphore *s, i32 count);
CM_DEF void cm_semaphore_release(cmSemaphore *s);
CM_DEF void cm_semaphore_wait   (cmSemaphore *s);

CM_END_EXTERN

#endif //CM_SEMAPHORE_H