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

#ifndef CM_DYNAMIC_ARRAY_H
#define CM_DYNAMIC_ARRAY_H

#include "memory.h"
#include "assert.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////////
//
// Dynamic Array (POD Types)
//
// NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
// Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!
//
// cmArray(Type) works like cmString or cmBuffer where the actual type is just a pointer to the first
// element.
//
///////////////////////////////////////////////////////////////////////////////////


// Available Procedures for cmArray(Type)
// cm_array_init
// cm_array_free
// cm_array_set_capacity
// cm_array_grow
// cm_array_append
// cm_array_appendv
// cm_array_pop
// cm_array_clear
// cm_array_resize
// cm_array_reserve
//

#if 0 // Example
void foo(void) {
	isize i;
	int test_values[] = {4, 2, 1, 7};
	cmAllocator a = cm_heap_allocator();
	cmArray(int) items;

	cm_array_init(items, a);

	cm_array_append(items, 1);
	cm_array_append(items, 4);
	cm_array_append(items, 9);
	cm_array_append(items, 16);

	items[1] = 3; // Manually set value
	              // NOTE: No array bounds checking

	for (i = 0; i < items.count; i++)
		cm_printf("%d\n", items[i]);
	// 1
	// 3
	// 9
	// 16

	cm_array_clear(items);

	cm_array_appendv(items, test_values, cm_count_of(test_values));
	for (i = 0; i < items.count; i++)
		cm_printf("%d\n", items[i]);
	// 4
	// 2
	// 1
	// 7

	cm_array_free(items);
}
#endif

typedef struct cmArrayHeader {
	cmAllocator allocator;
	isize       count;
	isize       capacity;
} cmArrayHeader;

// NOTE(bill): This thing is magic!
#define cmArray(Type) Type *

#ifndef CM_ARRAY_GROW_FORMULA
#define CM_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

CM_STATIC_ASSERT(CM_ARRAY_GROW_FORMULA(0) > 0);

#define CM_ARRAY_HEADER(x)    (cast(cmArrayHeader *)(x) - 1)
#define cm_array_allocator(x) (CM_ARRAY_HEADER(x)->allocator)
#define cm_array_count(x)     (CM_ARRAY_HEADER(x)->count)
#define cm_array_capacity(x)  (CM_ARRAY_HEADER(x)->capacity)

// TODO(bill): Have proper alignment!
#define cm_array_init_reserve(x, allocator_, cap) do { \
	void **cm__array_ = cast(void **)&(x); \
	cmArrayHeader *cm__ah = cast(cmArrayHeader *)cm_alloc(allocator_, cm_size_of(cmArrayHeader)+cm_size_of(*(x))*(cap)); \
	cm__ah->allocator = allocator_; \
	cm__ah->count = 0; \
	cm__ah->capacity = cap; \
	*cm__array_ = cast(void *)(cm__ah+1); \
} while (0)

// NOTE(bill): Give it an initial default capacity
#define cm_array_init(x, allocator) cm_array_init_reserve(x, allocator, CM_ARRAY_GROW_FORMULA(0))

#define cm_array_free(x) do { \
	cmArrayHeader *cm__ah = CM_ARRAY_HEADER(x); \
	cm_free(cm__ah->allocator, cm__ah); \
} while (0)

#define cm_array_set_capacity(x, capacity) do { \
	if (x) { \
		void **cm__array_ = cast(void **)&(x); \
		*cm__array_ = cm__array_set_capacity((x), (capacity), cm_size_of(*(x))); \
	} \
} while (0)

// NOTE(bill): Do not use the thing below directly, use the macro
CM_DEF void *cm__array_set_capacity(void *array, isize capacity, isize element_size);


// TODO(bill): Decide on a decent growing formula for cmArray
#define cm_array_grow(x, min_capacity) do { \
	isize new_capacity = CM_ARRAY_GROW_FORMULA(cm_array_capacity(x)); \
	if (new_capacity < (min_capacity)) \
		new_capacity = (min_capacity); \
	cm_array_set_capacity(x, new_capacity); \
} while (0)


#define cm_array_append(x, item) do { \
	if (cm_array_capacity(x) < cm_array_count(x)+1) \
		cm_array_grow(x, 0); \
	(x)[cm_array_count(x)++] = (item); \
} while (0)

#define cm_array_appendv(x, items, item_count) do { \
	cmArrayHeader *cm__ah = CM_ARRAY_HEADER(x); \
	CM_ASSERT(cm_size_of((items)[0]) == cm_size_of((x)[0])); \
	if (cm__ah->capacity < cm__ah->count+(item_count)) \
		cm_array_grow(x, cm__ah->count+(item_count)); \
	cm_memcopy(&(x)[cm__ah->count], (items), cm_size_of((x)[0])*(item_count));\
	cm__ah->count += (item_count); \
} while (0)



#define cm_array_pop(x)   do { CM_ASSERT(CM_ARRAY_HEADER(x)->count > 0); CM_ARRAY_HEADER(x)->count--; } while (0)
#define cm_array_clear(x) do { CM_ARRAY_HEADER(x)->count = 0; } while (0)

#define cm_array_resize(x, new_count) do { \
	if (CM_ARRAY_HEADER(x)->capacity < (new_count)) \
		cm_array_grow(x, (new_count)); \
	CM_ARRAY_HEADER(x)->count = (new_count); \
} while (0)


#define cm_array_reserve(x, new_capacity) do { \
	if (CM_ARRAY_HEADER(x)->capacity < (new_capacity)) \
		cm_array_set_capacity(x, new_capacity); \
} while (0)

#endif //CM_DYNAMIC_ARRAY_H