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

#ifndef CM_AFFINITY_H
#define CM_AFFINITY_H

#include "dll.h"
#include "arch.h"
#include "types.h"
#include "utils.h"

#if defined(CM_SYS_WINDOWS)
typedef struct cmAffinity {
	b32   is_accurate;
	isize core_count;
	isize thread_count;
	#define CM_WIN32_MAX_THREADS (8 * cm_size_of(usize))
	usize core_masks[CM_WIN32_MAX_THREADS];

} cmAffinity;

#elif defined(CM_SYS_OSX)
typedef struct cmAffinity {
	b32   is_accurate;
	isize core_count;
	isize thread_count;
	isize threads_per_core;
} cmAffinity;

#elif defined(CM_SYS_LINUX)
typedef struct cmAffinity {
	b32   is_accurate;
	isize core_count;
	isize thread_count;
	isize threads_per_core;
} cmAffinity;

#endif

CM_DEF void  cm_affinity_init                 (cmAffinity *a);
CM_DEF void  cm_affinity_destroy              (cmAffinity *a);
CM_DEF b32   cm_affinity_set                  (cmAffinity *a, isize core, isize thread);
CM_DEF isize cm_affinity_thread_count_for_core(cmAffinity *a, isize core);

#endif //CM_AFFINITY_H