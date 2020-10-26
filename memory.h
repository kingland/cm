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

#ifndef CM_MEMORY_H
#define CM_MEMORY_H

#include "dll.h"
#include "types.h"
#include "utils.h"


CM_BEGIN_EXTERN

//
// Memory
//

CM_DEF b32         	cm_is_power_of_two(isize x);

CM_DEF void 		*cm_align_forward(void *ptr, isize alignment);

CM_DEF void 		*cm_pointer_add(void *ptr, isize bytes);
CM_DEF void 		*cm_pointer_sub(void *ptr, isize bytes);
CM_DEF void const 	*cm_pointer_add_const(void const *ptr, isize bytes);
CM_DEF void const 	*cm_pointer_sub_const(void const *ptr, isize bytes);
CM_DEF isize       	cm_pointer_diff(void const *begin, void const *end);

CM_DEF void        	cm_zero_size(void *ptr, isize size);

#ifndef cm_zero_item
#define cm_zero_item(t)         cm_zero_size((t), cm_size_of(*(t))) // NOTE(bill): Pass pointer of struct
#define cm_zero_array(a, count) cm_zero_size((a), cm_size_of(*(a))*count)
#endif

CM_DEF void 		*cm_memcopy   (void *dest, void const *source, isize size);
CM_DEF void 		*cm_memmove   (void *dest, void const *source, isize size);
CM_DEF void 		*cm_memset    (void *data, u8 byte_value, isize size);
CM_DEF i32			cm_memcompare (void const *s1, void const *s2, isize size);
CM_DEF void			cm_memswap    (void *i, void *j, isize size);
CM_DEF void const 	*cm_memchr    (void const *data, u8 byte_value, isize size);
CM_DEF void const 	*cm_memrchr   (void const *data, u8 byte_value, isize size);


#ifndef cm_memcopy_array
#define cm_memcopy_array(dst, src, count) cm_memcopy((dst), (src), cm_size_of(*(dst))*(count))
#endif

#ifndef cm_memmove_array
#define cm_memmove_array(dst, src, count) cm_memmove((dst), (src), cm_size_of(*(dst))*(count))
#endif


////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
////////////////////////////////////////////////////////////////

typedef struct cmVirtualMemory {
	void *data;
	isize size;
} cmVirtualMemory;

CM_DEF cmVirtualMemory cm_virtual_memory(void *data, isize size);
CM_DEF cmVirtualMemory cm_vm_alloc      (void *addr, isize size);
CM_DEF b32             cm_vm_free       (cmVirtualMemory vm);
CM_DEF cmVirtualMemory cm_vm_trim       (cmVirtualMemory vm, isize lead_size, isize size);
CM_DEF b32             cm_vm_purge      (cmVirtualMemory vm);
CM_DEF isize           cm_virtual_memory_page_size(isize *alignment_out);


////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
////////////////////////////////////////////////////////////////

typedef enum cmAllocationType {
	cmAllocation_Alloc,
	cmAllocation_Free,
	cmAllocation_FreeAll,
	cmAllocation_Resize,
} cmAllocationType;

// NOTE(bill): This is useful so you can define an allocator of the same type and parameters
#define CM_ALLOCATOR_PROC(name)                         \
void *name(void *allocator_data, cmAllocationType type, \
           isize size, isize alignment,                 \
           void *old_memory, isize old_size,            \
           u64 flags)
typedef CM_ALLOCATOR_PROC(cmAllocatorProc);

typedef struct cmAllocator {
	cmAllocatorProc *proc;
	void *           data;
} cmAllocator;

typedef enum cmAllocatorFlag {
	cmAllocatorFlag_ClearToZero = CM_BIT(0),
} cmAllocatorFlag;

// TODO(bill): Is this a decent default alignment?
#ifndef CM_DEFAULT_MEMORY_ALIGNMENT
#define CM_DEFAULT_MEMORY_ALIGNMENT (2 * cm_size_of(void *))
#endif

#ifndef CM_DEFAULT_ALLOCATOR_FLAGS
#define CM_DEFAULT_ALLOCATOR_FLAGS (cmAllocatorFlag_ClearToZero)
#endif

CM_DEF void 		*cm_alloc_align	(cmAllocator a, isize size, isize alignment);
CM_DEF void 		*cm_alloc       (cmAllocator a, isize size);
CM_DEF void  		cm_free        	(cmAllocator a, void *ptr);
CM_DEF void  		cm_free_all    	(cmAllocator a);
CM_DEF void 		*cm_resize      (cmAllocator a, void *ptr, isize old_size, isize new_size);
CM_DEF void 		*cm_resize_align(cmAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
// TODO(bill): For gb_resize, should the use need to pass the old_size or only the new_size?

CM_DEF void 		*cm_alloc_copy      (cmAllocator a, void const *src, isize size);
CM_DEF void 		*cm_alloc_copy_align(cmAllocator a, void const *src, isize size, isize alignment);
CM_DEF char 		*cm_alloc_str       (cmAllocator a, char const *str);
CM_DEF char 		*cm_alloc_str_len   (cmAllocator a, char const *str, isize len);


// NOTE(bill): These are very useful and the type cast has saved me from numerous bugs
#ifndef cm_alloc_item
#define cm_alloc_item(allocator_, Type)         (Type *)cm_alloc(allocator_, cm_size_of(Type))
#define cm_alloc_array(allocator_, Type, count) (Type *)cm_alloc(allocator_, cm_size_of(Type) * (count))
#endif

// NOTE(bill): Use this if you don't need a "fancy" resize allocation
CM_DEF void *cm_default_resize_align(cmAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);



// TODO(bill): Probably use a custom heap allocator system that doesn't depend on malloc/free
// Base it off TCMalloc or something else? Or something entirely custom?
CM_DEF cmAllocator cm_heap_allocator(void);
CM_DEF CM_ALLOCATOR_PROC(cm_heap_allocator_proc);

// NOTE(bill): Yep, I use my own allocator system!
#ifndef cm_malloc
#define cm_malloc(sz) cm_alloc(cm_heap_allocator(), sz)
#define cm_mfree(ptr) cm_free(cm_heap_allocator(), ptr)
#endif



////////////////////////////////////////////////////////////////
// 
//  Arena Allocator
//
////////////////////////////////////////////////////////////////

typedef struct cmArena {
	cmAllocator backing;
	void *      physical_start;
	isize       total_size;
	isize       total_allocated;
	isize       temp_count;
} cmArena;

CM_DEF void 			cm_arena_init_from_memory   (cmArena *arena, void *start, isize size);
CM_DEF void 			cm_arena_init_from_allocator(cmArena *arena, cmAllocator backing, isize size);
CM_DEF void 			cm_arena_init_sub           (cmArena *arena, cmArena *parent_arena, isize size);
CM_DEF void 			cm_arena_free               (cmArena *arena);

CM_DEF isize 			cm_arena_alignment_of  (cmArena *arena, isize alignment);
CM_DEF isize 			cm_arena_size_remaining(cmArena *arena, isize alignment);
CM_DEF void  			cm_arena_check         (cmArena *arena);


// Allocation Types: alloc, free_all, resize
CM_DEF cmAllocator 		cm_arena_allocator(cmArena *arena);
CM_DEF CM_ALLOCATOR_PROC(cm_arena_allocator_proc);

typedef struct cmTempArenaMemory {
	cmArena *arena;
	isize    original_count;
} cmTempArenaMemory;

CM_DEF cmTempArenaMemory cm_temp_arena_memory_begin(cmArena *arena);
CM_DEF void              cm_temp_arena_memory_end  (cmTempArenaMemory tmp_mem);


///////////////////////////////////////////////////////////
//
// Pool Allocator
//
////////////////////////////////////////////////////////////////

typedef struct cmPool {
	cmAllocator backing;
	void *      physical_start;
	void *      free_list;
	isize       block_size;
	isize       block_align;
	isize       total_size;
} cmPool;

CM_DEF void 			cm_pool_init      (cmPool *pool, cmAllocator backing, isize num_blocks, isize block_size);
CM_DEF void 			cm_pool_init_align(cmPool *pool, cmAllocator backing, isize num_blocks, isize block_size, isize block_align);
CM_DEF void 			cm_pool_free      (cmPool *pool);

// Allocation Types: alloc, free
CM_DEF cmAllocator cm_pool_allocator(cmPool *pool);
CM_DEF CM_ALLOCATOR_PROC(cm_pool_allocator_proc);



// NOTE(bill): Used for allocators to keep track of sizes
typedef struct cmAllocationHeader {
	isize size;
} cmAllocationHeader;

CM_DEF cmAllocationHeader *cm_allocation_header     (void *data);
CM_DEF void                cm_allocation_header_fill(cmAllocationHeader *header, void *data, isize size);

// TODO(bill): Find better way of doing this without #if #elif etc.
#if defined(CM_ARCH_32)
#define CM_ISIZE_HIGH_BIT 0x80000000
#elif defined(CM_ARCH_64)
#define CM_ISIZE_HIGH_BIT 0x8000000000000000ll
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Free List Allocator
//
// IMPORTANT TODO(bill): Thoroughly test the free list allocator!
// NOTE(bill): This is a very shitty free list as it just picks the first free block not the best size
// as I am just being lazy. Also, I will probably remove it later; it's only here because why not?!
//
// NOTE(bill): I may also complete remove this if I completely implement a fixed heap allocator
//
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct cmFreeListBlock cmFreeListBlock;
struct cmFreeListBlock {
	cmFreeListBlock *next;
	isize            size;
};

typedef struct cmFreeList {
	void *           physical_start;
	isize            total_size;

	cmFreeListBlock *curr_block;

	isize            total_allocated;
	isize            allocation_count;
} cmFreeList;

CM_DEF void 			cm_free_list_init               (cmFreeList *fl, void *start, isize size);
CM_DEF void 			cm_free_list_init_from_allocator(cmFreeList *fl, cmAllocator backing, isize size);

// Allocation Types: alloc, free, free_all, resize
CM_DEF cmAllocator 		cm_free_list_allocator(cmFreeList *fl);
CM_DEF CM_ALLOCATOR_PROC(cm_free_list_allocator_proc);


///////////////////////////////////////////////////////////////
//
// Scratch Memory Allocator - Ring Buffer Based Arena
//
///////////////////////////////////////////////////////////////

typedef struct cmScratchMemory {
	void *physical_start;
	isize total_size;
	void *alloc_point;
	void *free_point;
} cmScratchMemory;

CM_DEF void 			cm_scratch_memory_init     (cmScratchMemory *s, void *start, isize size);
CM_DEF b32  			cm_scratch_memory_is_in_use(cmScratchMemory *s, void *ptr);


// Allocation Types: alloc, free, free_all, resize
CM_DEF cmAllocator 		cm_scratch_allocator(cmScratchMemory *s);
CM_DEF CM_ALLOCATOR_PROC(cm_scratch_allocator_proc);

// TODO(bill): Stack allocator
// TODO(bill): Fixed heap allocator
// TODO(bill): General heap allocator. Maybe a TCMalloc like clone?

CM_END_EXTERN

#endif //CM_MEMORY_H