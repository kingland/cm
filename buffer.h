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

#ifndef CM_BUFFER_H
#define CM_BUFFER_H

#include "memory.h"
#include "debug.h"

CM_BEGIN_EXTERN
/*
// Fixed Capacity Buffer (POD Types)
//
//
// cmBuffer(Type) works like cmString or cmArray where the actual type is just a pointer to the first
// element.
*/

typedef struct cmBufferHeader {
	isize count;
	isize capacity;
} cmBufferHeader;

#define cmBuffer(Type) Type *

#define CM_BUFFER_HEADER(x)   (cast(cmBufferHeader *)(x) - 1)
#define cm_buffer_count(x)    (CM_BUFFER_HEADER(x)->count)
#define cm_buffer_capacity(x) (CM_BUFFER_HEADER(x)->capacity)

#define cm_buffer_init(x, allocator, cap) do { \
	void **nx = cast(void **)&(x); \
	cmBufferHeader *cm__bh = cast(cmBufferHeader *)cm_alloc((allocator), (cap)*cm_size_of(*(x))); \
	cm__bh->count = 0; \
	cm__bh->capacity = cap; \
	*nx = cast(void *)(cm__bh+1); \
} while (0)


#define cm_buffer_free(x, allocator) (cm_free(allocator, CM_BUFFER_HEADER(x)))

#define cm_buffer_append(x, item) do { (x)[cm_buffer_count(x)++] = (item); } while (0)

#define cm_buffer_appendv(x, items, item_count) do { \
	CM_ASSERT(cm_size_of(*(items)) == cm_size_of(*(x))); \
	CM_ASSERT(cm_buffer_count(x)+item_count <= cm_buffer_capacity(x)); \
	cm_memcopy(&(x)[cm_buffer_count(x)], (items), cm_size_of(*(x))*(item_count)); \
	cm_buffer_count(x) += (item_count); \
} while (0)

#define cm_buffer_pop(x)   do { CM_ASSERT(cm_buffer_count(x) > 0); cm_buffer_count(x)--; } while (0)
#define cm_buffer_clear(x) do { cm_buffer_count(x) = 0; } while (0)

CM_END_EXTERN

#endif //CM_BUFFER_H