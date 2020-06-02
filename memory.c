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

#include "memory.h"
#include "debug.h"
#include "arch.h"
#include "header.h"
#include "char.h"
#include "print.h"

CM_BEGIN_EXTERN

b32 
cm_is_power_of_two(isize x) {
	if (x <= 0)
		return false;
	return !(x & (x-1));
}

cm_inline void *
cm_align_forward(void *ptr, isize alignment) {
	uintptr p;

	CM_ASSERT(cm_is_power_of_two(alignment));

	p = cast(uintptr)ptr;
	return cast(void *)((p + (alignment-1)) &~ (alignment-1));
}

cm_inline void *      
cm_pointer_add(void *ptr, isize bytes) { 
    return cast(void *)(cast(u8 *)ptr + bytes); 
}

cm_inline void *      
cm_pointer_sub(void *ptr, isize bytes){ 
    return cast(void *)(cast(u8 *)ptr - bytes); 
}

cm_inline void const *
cm_pointer_add_const(void const *ptr, isize bytes){ 
    return cast(void const *)(cast(u8 const *)ptr + bytes); 
}

cm_inline void const *
cm_pointer_sub_const(void const *ptr, isize bytes){ 
    return cast(void const *)(cast(u8 const *)ptr - bytes); 
}

cm_inline isize       
cm_pointer_diff(void const *begin, void const *end) { 
    return cast(isize)(cast(u8 const *)end - cast(u8 const *)begin); 
}

cm_inline void 
cm_zero_size(void *ptr, isize size) { 
    cm_memset(ptr, 0, size); 
}

#if defined(_MSC_VER)
#pragma intrinsic(__movsb)
#endif


cm_inline void *
cm_memcopy(void *dest, void const *source, isize n) {

#if defined(_MSC_VER)
if (dest == NULL) {
		return NULL;
	}
	// TODO(bill): Is this good enough?
	__movsb(cast(u8 *)dest, cast(u8 *)source, n);
// #elif defined(CM_SYS_OSX) || defined(CM_SYS_UNIX)
	// NOTE(zangent): I assume there's a reason this isn't being used elsewhere,
	//   but casting pointers as arguments to an __asm__ call is considered an
	//   error on MacOS and (I think) Linux
	// TODO(zangent): Figure out how to refactor the asm code so it works on MacOS,
	//   since this is probably not the way the author intended this to work.
	// memcpy(dest, source, n);
#elif defined(CM_CPU_X86)
	if (dest == NULL) {
		return NULL;
	}

	void *dest_copy = dest;
	__asm__ __volatile__("rep movsb" : "+D"(dest_copy), "+S"(source), "+c"(n) : : "memory");
#else
    u8 *d = cast(u8 *)dest;
	u8 const *s = cast(u8 const *)source;
	u32 w, x;

	if (dest == NULL) {
		return NULL;
	}

	for (; cast(uintptr)s % 4 && n; n--) {
		*d++ = *s++;
	}

	if (cast(uintptr)d % 4 == 0) {
		for (; n >= 16;
		     s += 16, d += 16, n -= 16) {
			*cast(u32 *)(d+ 0) = *cast(u32 *)(s+ 0);
			*cast(u32 *)(d+ 4) = *cast(u32 *)(s+ 4);
			*cast(u32 *)(d+ 8) = *cast(u32 *)(s+ 8);
			*cast(u32 *)(d+12) = *cast(u32 *)(s+12);
		}
		if (n & 8) {
			*cast(u32 *)(d+0) = *cast(u32 *)(s+0);
			*cast(u32 *)(d+4) = *cast(u32 *)(s+4);
			d += 8;
			s += 8;
		}
		if (n&4) {
			*cast(u32 *)(d+0) = *cast(u32 *)(s+0);
			d += 4;
			s += 4;
		}
		if (n&2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n&1) {
			*d = *s;
		}
		return dest;
	}

	if (n >= 32) {
	#if __BYTE_ORDER == __BIG_ENDIAN
	#define LS <<
	#define RS >>
	#else
	#define LS >>
	#define RS <<
	#endif
		switch (cast(uintptr)d % 4) {
		case 1: {
			w = *cast(u32 *)s;
			*d++ = *s++;
			*d++ = *s++;
			*d++ = *s++;
			n -= 3;
			while (n > 16) {
				x = *cast(u32 *)(s+1);
				*cast(u32 *)(d+0)  = (w LS 24) | (x RS 8);
				w = *cast(u32 *)(s+5);
				*cast(u32 *)(d+4)  = (x LS 24) | (w RS 8);
				x = *cast(u32 *)(s+9);
				*cast(u32 *)(d+8)  = (w LS 24) | (x RS 8);
				w = *cast(u32 *)(s+13);
				*cast(u32 *)(d+12) = (x LS 24) | (w RS 8);

				s += 16;
				d += 16;
				n -= 16;
			}
		} break;
		case 2: {
			w = *cast(u32 *)s;
			*d++ = *s++;
			*d++ = *s++;
			n -= 2;
			while (n > 17) {
				x = *cast(u32 *)(s+2);
				*cast(u32 *)(d+0)  = (w LS 16) | (x RS 16);
				w = *cast(u32 *)(s+6);
				*cast(u32 *)(d+4)  = (x LS 16) | (w RS 16);
				x = *cast(u32 *)(s+10);
				*cast(u32 *)(d+8)  = (w LS 16) | (x RS 16);
				w = *cast(u32 *)(s+14);
				*cast(u32 *)(d+12) = (x LS 16) | (w RS 16);

				s += 16;
				d += 16;
				n -= 16;
			}
		} break;
		case 3: {
			w = *cast(u32 *)s;
			*d++ = *s++;
			n -= 1;
			while (n > 18) {
				x = *cast(u32 *)(s+3);
				*cast(u32 *)(d+0)  = (w LS 8) | (x RS 24);
				w = *cast(u32 *)(s+7);
				*cast(u32 *)(d+4)  = (x LS 8) | (w RS 24);
				x = *cast(u32 *)(s+11);
				*cast(u32 *)(d+8)  = (w LS 8) | (x RS 24);
				w = *cast(u32 *)(s+15);
				*cast(u32 *)(d+12) = (x LS 8) | (w RS 24);

				s += 16;
				d += 16;
				n -= 16;
			}
		} break;
		default: break; // NOTE(bill): Do nowt!
		}
	#undef LS
	#undef RS
		if (n & 16) {
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		}
		if (n & 8) {
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		}
		if (n & 4) {
			*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		}
		if (n & 2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n & 1) {
			*d = *s;
		}
	}
#endif
	return dest;
}

cm_inline void *
cm_memmove(void *dest, void const *source, isize n) {
	u8 *d = cast(u8 *)dest;
	u8 const *s = cast(u8 const *)source;

	if (dest == NULL) {
		return NULL;
	}

	if (d == s) {
		return d;
	}
	if (s+n <= d || d+n <= s) { // NOTE(bill): Non-overlapping
		return cm_memcopy(d, s, n);
	}

	if (d < s) {
		if (cast(uintptr)s % cm_size_of(isize) == cast(uintptr)d % cm_size_of(isize)) {
			while (cast(uintptr)d % cm_size_of(isize)) {
				if (!n--) return dest;
				*d++ = *s++;
			}
			while (n>=cm_size_of(isize)) {
				*cast(isize *)d = *cast(isize *)s;
				n -= cm_size_of(isize);
				d += cm_size_of(isize);
				s += cm_size_of(isize);
			}
		}
		for (; n; n--) *d++ = *s++;
	} else {
		if ((cast(uintptr)s % cm_size_of(isize)) == (cast(uintptr)d % cm_size_of(isize))) {
			while (cast(uintptr)(d+n) % cm_size_of(isize)) {
				if (!n--)
					return dest;
				d[n] = s[n];
			}
			while (n >= cm_size_of(isize)) {
				n -= cm_size_of(isize);
				*cast(isize *)(d+n) = *cast(isize *)(s+n);
			}
		}
		while (n) n--, d[n] = s[n];
	}

	return dest;
}

cm_inline 
void *cm_memset(void *dest, u8 c, isize n) {
	u8 *s = cast(u8 *)dest;
	isize k;
	u32 c32 = ((u32)-1)/255 * c;

	if (dest == NULL) {
		return NULL;
	}

	if (n == 0)
		return dest;
	s[0] = s[n-1] = c;
	if (n < 3)
		return dest;
	s[1] = s[n-2] = c;
	s[2] = s[n-3] = c;
	if (n < 7)
		return dest;
	s[3] = s[n-4] = c;
	if (n < 9)
		return dest;

	k = -cast(intptr)s & 3;
	s += k;
	n -= k;
	n &= -4;

	*cast(u32 *)(s+0) = c32;
	*cast(u32 *)(s+n-4) = c32;
	if (n < 9) {
		return dest;
	}
	*cast(u32 *)(s +  4)    = c32;
	*cast(u32 *)(s +  8)    = c32;
	*cast(u32 *)(s+n-12) = c32;
	*cast(u32 *)(s+n- 8) = c32;
	if (n < 25) {
		return dest;
	}
	*cast(u32 *)(s + 12) = c32;
	*cast(u32 *)(s + 16) = c32;
	*cast(u32 *)(s + 20) = c32;
	*cast(u32 *)(s + 24) = c32;
	*cast(u32 *)(s+n-28) = c32;
	*cast(u32 *)(s+n-24) = c32;
	*cast(u32 *)(s+n-20) = c32;
	*cast(u32 *)(s+n-16) = c32;

	k = 24 + (cast(uintptr)s & 4);
	s += k;
	n -= k;


	{
		u64 c64 = (cast(u64)c32 << 32) | c32;
		while (n > 31) {
			*cast(u64 *)(s+0) = c64;
			*cast(u64 *)(s+8) = c64;
			*cast(u64 *)(s+16) = c64;
			*cast(u64 *)(s+24) = c64;

			n -= 32;
			s += 32;
		}
	}

	return dest;
}

cm_inline i32 
cm_memcompare(void const *s1, void const *s2, isize size) {
	// TODO(bill): Heavily optimize
	u8 const *s1p8 = cast(u8 const *)s1;
	u8 const *s2p8 = cast(u8 const *)s2;

	if (s1 == NULL || s2 == NULL) {
		return 0;
	}

	while (size--) {
		if (*s1p8 != *s2p8) {
			return (*s1p8 - *s2p8);
		}
		s1p8++, s2p8++;
	}
	return 0;
}

void 
cm_memswap(void *i, void *j, isize size) {
	if (i == j) return;

	if (size == 4) {
		cm_swap(u32, *cast(u32 *)i, *cast(u32 *)j);
	} else if (size == 8) {
		cm_swap(u64, *cast(u64 *)i, *cast(u64 *)j);
	} else if (size < 8) {
		u8 *a = cast(u8 *)i;
		u8 *b = cast(u8 *)j;
		if (a != b) {
			while (size--) {
				cm_swap(u8, *a, *b);
				a++, b++;
			}
		}
	} else {
		char buffer[256];

		// TODO(bill): Is the recursion ever a problem?
		while (size > cm_size_of(buffer)) {
			cm_memswap(i, j, cm_size_of(buffer));
			i = cm_pointer_add(i, cm_size_of(buffer));
			j = cm_pointer_add(j, cm_size_of(buffer));
			size -= cm_size_of(buffer);
		}

		cm_memcopy(buffer, i,      size);
		cm_memcopy(i,      j,      size);
		cm_memcopy(j,      buffer, size);
	}
}

#define CM__ONES        (cast(usize)-1/U8_MAX)
#define CM__HIGHS       (CM__ONES * (U8_MAX/2+1))
#define CM__HAS_ZERO(x) ((x)-CM__ONES & ~(x) & CM__HIGHS)

void const *
cm_memchr(void const *data, u8 c, isize n) {
	u8 const *s = cast(u8 const *)data;
	while ((cast(uintptr)s & (sizeof(usize)-1)) &&
	       n && *s != c) {
		s++;
		n--;
	}
	if (n && *s != c) {
		isize const *w;
		isize k = CM__ONES * c;
		w = cast(isize const *)s;
		while (n >= cm_size_of(isize) && !CM__HAS_ZERO(*w ^ k)) {
			w++;
			n -= cm_size_of(isize);
		}
		s = cast(u8 const *)w;
		while (n && *s != c) {
			s++;
			n--;
		}
	}

	return n ? cast(void const *)s : NULL;
}

void const *
cm_memrchr(void const *data, u8 c, isize n) {
	u8 const *s = cast(u8 const *)data;
	while (n--) {
		if (s[n] == c)
			return cast(void const *)(s + n);
	}
	return NULL;
}



cm_inline void *
cm_alloc_align (cmAllocator a, isize size, isize alignment){ 
    return a.proc(a.data, cmAllocation_Alloc, size, alignment, NULL, 0, CM_DEFAULT_ALLOCATOR_FLAGS); 
}

cm_inline void *
cm_alloc(cmAllocator a, isize size){ 
    return cm_alloc_align(a, size, CM_DEFAULT_MEMORY_ALIGNMENT); 
}

cm_inline void  
cm_free(cmAllocator a, void *ptr){ 
    if (ptr != NULL) a.proc(a.data, cmAllocation_Free, 0, 0, ptr, 0, CM_DEFAULT_ALLOCATOR_FLAGS); 
}

cm_inline void  
cm_free_all(cmAllocator a){ 
    a.proc(a.data, cmAllocation_FreeAll, 0, 0, NULL, 0, CM_DEFAULT_ALLOCATOR_FLAGS); 
}

cm_inline void *
cm_resize(cmAllocator a, void *ptr, isize old_size, isize new_size){ 
    return cm_resize_align(a, ptr, old_size, new_size, CM_DEFAULT_MEMORY_ALIGNMENT); 
}

cm_inline void *
cm_resize_align(cmAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) { 
    return a.proc(a.data, cmAllocation_Resize, new_size, alignment, ptr, old_size, CM_DEFAULT_ALLOCATOR_FLAGS); 
}

cm_inline void *
cm_alloc_copy(cmAllocator a, void const *src, isize size) {
	return cm_memcopy(cm_alloc(a, size), src, size);
}

cm_inline void *
cm_alloc_copy_align(cmAllocator a, void const *src, isize size, isize alignment) {
	return cm_memcopy(cm_alloc_align(a, size, alignment), src, size);
}

cm_inline char *
cm_alloc_str(cmAllocator a, char const *str) {
	return cm_alloc_str_len(a, str, cm_strlen(str));
}

cm_inline char *
cm_alloc_str_len(cmAllocator a, char const *str, isize len) {
	char *result;
	result = cast(char *)cm_alloc_copy(a, str, len+1);
	result[len] = '\0';
	return result;
}


cm_inline void *
cm_default_resize_align(cmAllocator a, void *old_memory, isize old_size, isize new_size, isize alignment) {
	if (!old_memory) return cm_alloc_align(a, new_size, alignment);

	if (new_size == 0) {
		cm_free(a, old_memory);
		return NULL;
	}

	if (new_size < old_size)
		new_size = old_size;

	if (old_size == new_size) {
		return old_memory;
	} else {
		void *new_memory = cm_alloc_align(a, new_size, alignment);
		if (!new_memory) return NULL;
		cm_memmove(new_memory, old_memory, CM_MIN(new_size, old_size));
		cm_free(a, old_memory);
		return new_memory;
	}
}

cm_inline cmAllocator 
cm_heap_allocator(void) {
	cmAllocator a;
	a.proc = cm_heap_allocator_proc;
	a.data = NULL;
	return a;
}

CM_ALLOCATOR_PROC(cm_heap_allocator_proc) {
	void *ptr = NULL;
	cm_unused(allocator_data);
	cm_unused(old_size);
// TODO(bill): Throughly test!
	switch (type) {
#if defined(CM_COMPILER_MSVC)
	case cmAllocation_Alloc:
		ptr = _aligned_malloc(size, alignment);
		if (flags & cmAllocatorFlag_ClearToZero)
			cm_zero_size(ptr, size);
		break;
	case cmAllocation_Free:
		_aligned_free(old_memory);
		break;
	case cmAllocation_Resize:
		ptr = _aligned_realloc(old_memory, size, alignment);
		break;

#elif defined(CM_SYS_LINUX)
	// TODO(bill): *nix version that's decent
	case cmAllocation_Alloc: {
		ptr = aligned_alloc(alignment, size);
		// ptr = malloc(size+alignment);

		if (flags & cmAllocatorFlag_ClearToZero) {
			cm_zero_size(ptr, size);
		}
	} break;

	case cmAllocation_Free: {
		free(old_memory);
	} break;

	case cmAllocation_Resize: {
		// ptr = realloc(old_memory, size);
		ptr = cm_default_resize_align(cm_heap_allocator(), old_memory, old_size, size, alignment);
	} break;
#else
	// TODO(bill): *nix version that's decent
	case cmAllocation_Alloc: {
		posix_memalign(&ptr, alignment, size);

		if (flags & gbAllocatorFlag_ClearToZero) {
			cm_zero_size(ptr, size);
		}
	} break;

	case cmAllocation_Free: {
		free(old_memory);
	} break;

	case cmAllocation_Resize: {
		ptr = cm_default_resize_align(cm_heap_allocator(), old_memory, old_size, size, alignment);
	} break;
#endif

	case cmAllocation_FreeAll:
		break;
	}

	return ptr;
}


////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
//

cmVirtualMemory 
cm_virtual_memory(void *data, isize size) {
	cmVirtualMemory vm;
	vm.data = data;
	vm.size = size;
	return vm;
}


#if defined(CM_SYS_WINDOWS)
cm_inline cmVirtualMemory 
cm_vm_alloc(void *addr, isize size) {
	cmVirtualMemory vm;
	CM_ASSERT(size > 0);
	vm.data = VirtualAlloc(addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	vm.size = size;
	return vm;
}

cm_inline b32 
cm_vm_free(cmVirtualMemory vm) {
	MEMORY_BASIC_INFORMATION info;
	while (vm.size > 0) {
		if (VirtualQuery(vm.data, &info, cm_size_of(info)) == 0)
			return false;
		if (info.BaseAddress != vm.data ||
		    info.AllocationBase != vm.data ||
		    info.State != MEM_COMMIT || info.RegionSize > cast(usize)vm.size) {
			return false;
		}
		if (VirtualFree(vm.data, 0, MEM_RELEASE) == 0)
			return false;
		vm.data = cm_pointer_add(vm.data, info.RegionSize);
		vm.size -= info.RegionSize;
	}
	return true;
}

cm_inline cmVirtualMemory 
cm_vm_trim(cmVirtualMemory vm, isize lead_size, isize size) {
	cmVirtualMemory new_vm = {0};
	void *ptr;
	CM_ASSERT(vm.size >= lead_size + size);

	ptr = cm_pointer_add(vm.data, lead_size);

	cm_vm_free(vm);
	new_vm = cm_vm_alloc(ptr, size);
	if (new_vm.data == ptr)
		return new_vm;
	if (new_vm.data)
		cm_vm_free(new_vm);
	return new_vm;
}

cm_inline b32 
cm_vm_purge(cmVirtualMemory vm) {
	VirtualAlloc(vm.data, vm.size, MEM_RESET, PAGE_READWRITE);
	// NOTE(bill): Can this really fail?
	return true;
}

isize 
cm_virtual_memory_page_size(isize *alignment_out) {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	if (alignment_out) *alignment_out = info.dwAllocationGranularity;
	return info.dwPageSize;
}

#else

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

cm_inline cmVirtualMemory 
cm_vm_alloc(void *addr, isize size) {
	cmVirtualMemory vm;
	CM_ASSERT(size > 0);
	vm.data = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	vm.size = size;
	return vm;
}

cm_inline b32 
cm_vm_free(cmVirtualMemory vm) {
	munmap(vm.data, vm.size);
	return true;
}

cm_inline cmVirtualMemory 
cm_vm_trim(cmVirtualMemory vm, isize lead_size, isize size) {
	void *ptr;
	isize trail_size;
	CM_ASSERT(vm.size >= lead_size + size);

	ptr = cm_pointer_add(vm.data, lead_size);
	trail_size = vm.size - lead_size - size;

	if (lead_size != 0)
		cm_vm_free(cm_virtual_memory(vm.data, lead_size));
	if (trail_size != 0)
		cm_vm_free(cm_virtual_memory(ptr, trail_size));
	return cm_virtual_memory(ptr, size);

}

cm_inline b32 
cm_vm_purge(cmVirtualMemory vm) {
	int err = madvise(vm.data, vm.size, MADV_DONTNEED);
	return err != 0;
}

isize 
cm_virtual_memory_page_size(isize *alignment_out) {
	// TODO(bill): Is this always true?
	isize result = cast(isize)sysconf(_SC_PAGE_SIZE);
	if (alignment_out) *alignment_out = result;
	return result;
}

#endif

/////////////////////////////////////////////////////////////////
//
// Arena Allocator
//

cm_inline void 
cm_arena_init_from_memory(cmArena *arena, void *start, isize size) {
	arena->backing.proc    = NULL;
	arena->backing.data    = NULL;
	arena->physical_start  = start;
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

cm_inline void 
cm_arena_init_from_allocator(cmArena *arena, cmAllocator backing, isize size) {
	arena->backing         = backing;
	arena->physical_start  = cm_alloc(backing, size); // NOTE(bill): Uses default alignment
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

cm_inline void 
cm_arena_init_sub(cmArena *arena, cmArena *parent_arena, isize size) { 
    cm_arena_init_from_allocator(arena, cm_arena_allocator(parent_arena), size); 
}

cm_inline void 
cm_arena_free(cmArena *arena) {
	if (arena->backing.proc) {
		cm_free(arena->backing, arena->physical_start);
		arena->physical_start = NULL;
	}
}

cm_inline isize 
cm_arena_alignment_of(cmArena *arena, isize alignment) {
	isize alignment_offset, result_pointer, mask;
	CM_ASSERT(cm_is_power_of_two(alignment));

	alignment_offset = 0;
	result_pointer = cast(isize)arena->physical_start + arena->total_allocated;
	mask = alignment - 1;
	if (result_pointer & mask)
		alignment_offset = alignment - (result_pointer & mask);

	return alignment_offset;
}

cm_inline isize 
cm_arena_size_remaining(cmArena *arena, isize alignment) {
	isize result = arena->total_size - (arena->total_allocated + cm_arena_alignment_of(arena, alignment));
	return result;
}

cm_inline void 
cm_arena_check(cmArena *arena) { CM_ASSERT(arena->temp_count == 0); }

cm_inline cmAllocator 
cm_arena_allocator(cmArena *arena) {
	cmAllocator allocator;
	allocator.proc = cm_arena_allocator_proc;
	allocator.data = arena;
	return allocator;
}

CM_ALLOCATOR_PROC(cm_arena_allocator_proc) {
	cmArena *arena = cast(cmArena *)allocator_data;
	void *ptr = NULL;

	cm_unused(old_size);

	switch (type) {
	case cmAllocation_Alloc: {
		void *end = cm_pointer_add(arena->physical_start, arena->total_allocated);
		isize total_size = size + alignment;

		// NOTE(bill): Out of memory
		if (arena->total_allocated + total_size > cast(isize)arena->total_size) {
			cm_printf_err("Arena out of memory\n");
			return NULL;
		}

		ptr = cm_align_forward(end, alignment);
		arena->total_allocated += total_size;
		if (flags & cmAllocatorFlag_ClearToZero)
			cm_zero_size(ptr, size);
	} break;

	case cmAllocation_Free:
		// NOTE(bill): Free all at once
		// Use Temp_Arena_Memory if you want to free a block
		break;

	case cmAllocation_FreeAll:
		arena->total_allocated = 0;
		break;

	case cmAllocation_Resize: {
		// TODO(bill): Check if ptr is on top of stack and just extend
		cmAllocator a = cm_arena_allocator(arena);
		ptr = cm_default_resize_align(a, old_memory, old_size, size, alignment);
	} break;
	}
	return ptr;
}


cm_inline cmTempArenaMemory 
cm_temp_arena_memory_begin(cmArena *arena) {
	cmTempArenaMemory tmp;
	tmp.arena = arena;
	tmp.original_count = arena->total_allocated;
	arena->temp_count++;
	return tmp;
}

cm_inline void 
cm_temp_arena_memory_end(cmTempArenaMemory tmp) {
	CM_ASSERT_MSG(tmp.arena->total_allocated >= tmp.original_count,
	              "%td >= %td", tmp.arena->total_allocated, tmp.original_count);
	CM_ASSERT(tmp.arena->temp_count > 0);
	tmp.arena->total_allocated = tmp.original_count;
	tmp.arena->temp_count--;
}

//////////////////////////////////////////////////////////////////////
//
// Pool Allocator
//

cm_inline void 
cm_pool_init(cmPool *pool, cmAllocator backing, isize num_blocks, isize block_size) {
	cm_pool_init_align(pool, backing, num_blocks, block_size, CM_DEFAULT_MEMORY_ALIGNMENT);
}

void 
cm_pool_init_align(cmPool *pool, cmAllocator backing, isize num_blocks, isize block_size, isize block_align) {
	isize actual_block_size, pool_size, block_index;
	void *data, *curr;
	uintptr *end;

	cm_zero_item(pool);

	pool->backing = backing;
	pool->block_size = block_size;
	pool->block_align = block_align;

	actual_block_size = block_size + block_align;
	pool_size = num_blocks * actual_block_size;

	data = cm_alloc_align(backing, pool_size, block_align);

	// NOTE(bill): Init intrusive freelist
	curr = data;
	for (block_index = 0; block_index < num_blocks-1; block_index++) {
		uintptr *next = cast(uintptr *)curr;
		*next = cast(uintptr)curr + actual_block_size;
		curr = cm_pointer_add(curr, actual_block_size);
	}

	end  = cast(uintptr *)curr;
	*end = cast(uintptr)NULL;

	pool->physical_start = data;
	pool->free_list      = data;
}

cm_inline void 
cm_pool_free(cmPool *pool) {
	if (pool->backing.proc) {
		cm_free(pool->backing, pool->physical_start);
	}
}

cm_inline cmAllocator 
cm_pool_allocator(cmPool *pool) {
	cmAllocator allocator;
	allocator.proc = cm_pool_allocator_proc;
	allocator.data = pool;
	return allocator;
}

CM_ALLOCATOR_PROC(cm_pool_allocator_proc) {
	cmPool *pool = cast(cmPool *)allocator_data;
	void *ptr = NULL;

	cm_unused(old_size);

	switch (type) {
	case cmAllocation_Alloc: {
		uintptr next_free;
		CM_ASSERT(size      == pool->block_size);
		CM_ASSERT(alignment == pool->block_align);
		CM_ASSERT(pool->free_list != NULL);

		next_free = *cast(uintptr *)pool->free_list;
		ptr = pool->free_list;
		pool->free_list = cast(void *)next_free;
		pool->total_size += pool->block_size;
		if (flags & cmAllocatorFlag_ClearToZero)
			cm_zero_size(ptr, size);
	} break;

	case cmAllocation_Free: {
		uintptr *next;
		if (old_memory == NULL) return NULL;

		next = cast(uintptr *)old_memory;
		*next = cast(uintptr)pool->free_list;
		pool->free_list = old_memory;
		pool->total_size -= pool->block_size;
	} break;

	case cmAllocation_FreeAll:
		// TODO(bill):
		break;

	case cmAllocation_Resize:
		// NOTE(bill): Cannot resize
		CM_PANIC("You cannot resize something allocated by with a pool.");
		break;
	}

	return ptr;
}

cm_inline cmAllocationHeader *
cm_allocation_header(void *data) {
	isize *p = cast(isize *)data;
	while (p[-1] == cast(isize)(-1)) {
		p--;
	}
	return cast(cmAllocationHeader *)p - 1;
}

cm_inline void 
cm_allocation_header_fill(cmAllocationHeader *header, void *data, isize size) {
	isize *ptr;
	header->size = size;
	ptr = cast(isize *)(header + 1);
	while (cast(void *)ptr < data) {
		*ptr++ = cast(isize)(-1);
	}
}

///////////////////////////////////////////////////////////////////////
//
// Free List Allocator
//

cm_inline void 
cm_free_list_init(cmFreeList *fl, void *start, isize size) {
	CM_ASSERT(size > cm_size_of(cmFreeListBlock));

	fl->physical_start   = start;
	fl->total_size       = size;
	fl->curr_block       = cast(cmFreeListBlock *)start;
	fl->curr_block->size = size;
	fl->curr_block->next = NULL;
}

cm_inline void 
cm_free_list_init_from_allocator(cmFreeList *fl, cmAllocator backing, isize size) {
	void *start = cm_alloc(backing, size);
	cm_free_list_init(fl, start, size);
}

cm_inline cmAllocator 
cm_free_list_allocator(cmFreeList *fl) {
	cmAllocator a;
	a.proc = cm_free_list_allocator_proc;
	a.data = fl;
	return a;
}

CM_ALLOCATOR_PROC(cm_free_list_allocator_proc) {
	cmFreeList *fl = cast(cmFreeList *)allocator_data;
	void *ptr = NULL;

	CM_ASSERT_NOT_NULL(fl);

	switch (type) {
	case cmAllocation_Alloc: {
		cmFreeListBlock *prev_block = NULL;
		cmFreeListBlock *curr_block = fl->curr_block;

		while (curr_block) {
			isize total_size;
			cmAllocationHeader *header;

			total_size = size + alignment + cm_size_of(cmAllocationHeader);

			if (curr_block->size < total_size) {
				prev_block = curr_block;
				curr_block = curr_block->next;
				continue;
			}

			if (curr_block->size - total_size <= cm_size_of(cmAllocationHeader)) {
				total_size = curr_block->size;

				if (prev_block)
					prev_block->next = curr_block->next;
				else
					fl->curr_block = curr_block->next;
			} else {
				// NOTE(bill): Create a new block for the remaining memory
				cmFreeListBlock *next_block;
				next_block = cast(cmFreeListBlock *)cm_pointer_add(curr_block, total_size);

				CM_ASSERT(cast(void *)next_block < cm_pointer_add(fl->physical_start, fl->total_size));

				next_block->size = curr_block->size - total_size;
				next_block->next = curr_block->next;

				if (prev_block)
					prev_block->next = next_block;
				else
					fl->curr_block = next_block;
			}


			// TODO(bill): Set Header Info
			header = cast(cmAllocationHeader *)curr_block;
			ptr = cm_align_forward(header+1, alignment);
			cm_allocation_header_fill(header, ptr, size);

			fl->total_allocated += total_size;
			fl->allocation_count++;


			if (flags & cmAllocatorFlag_ClearToZero)
				cm_zero_size(ptr, size);
			return ptr;
		}
		// NOTE(bill): if ptr == NULL, ran out of free list memory! FUCK!
		return NULL;
	} break;

	case cmAllocation_Free: {
		cmAllocationHeader *header = cm_allocation_header(old_memory);
		isize block_size = header->size;
		uintptr block_start, block_end;
		cmFreeListBlock *prev_block = NULL;
		cmFreeListBlock *curr_block = fl->curr_block;

		block_start = cast(uintptr)header;
		block_end   = cast(uintptr)block_start + block_size;

		while (curr_block) {
			if (cast(uintptr)curr_block >= block_end)
				break;
			prev_block = curr_block;
			curr_block = curr_block->next;
		}

		if (prev_block == NULL) {
			prev_block = cast(cmFreeListBlock *)block_start;
			prev_block->size = block_size;
			prev_block->next = fl->curr_block;

			fl->curr_block = prev_block;
		} else if ((cast(uintptr)prev_block + prev_block->size) == block_start) {
			prev_block->size += block_size;
		} else {
			cmFreeListBlock *tmp = cast(cmFreeListBlock *)block_start;
			tmp->size = block_size;
			tmp->next = prev_block->next;
			prev_block->next = tmp;

			prev_block = tmp;
		}

		if (curr_block && (cast(uintptr)curr_block == block_end)) {
			prev_block->size += curr_block->size;
			prev_block->next = curr_block->next;
		}

		fl->allocation_count--;
		fl->total_allocated -= block_size;
	} break;

	case cmAllocation_FreeAll:
		cm_free_list_init(fl, fl->physical_start, fl->total_size);
		break;

	case cmAllocation_Resize:
		ptr = cm_default_resize_align(cm_free_list_allocator(fl), old_memory, old_size, size, alignment);
		break;
	}

	return ptr;
}

void 
cm_scratch_memory_init(cmScratchMemory *s, void *start, isize size) {
	s->physical_start = start;
	s->total_size     = size;
	s->alloc_point    = start;
	s->free_point     = start;
}

b32 
cm_scratch_memory_is_in_use(cmScratchMemory *s, void *ptr) {
	if (s->free_point == s->alloc_point) return false;
	if (s->alloc_point > s->free_point)
		return ptr >= s->free_point && ptr < s->alloc_point;
	return ptr >= s->free_point || ptr < s->alloc_point;
}

cmAllocator 
cm_scratch_allocator(cmScratchMemory *s) {
	cmAllocator a;
	a.proc = cm_scratch_allocator_proc;
	a.data = s;
	return a;
}

CM_ALLOCATOR_PROC(cm_scratch_allocator_proc) {
	cmScratchMemory *s = cast(cmScratchMemory *)allocator_data;
	void *ptr = NULL;
	CM_ASSERT_NOT_NULL(s);

	switch (type) {
	case cmAllocation_Alloc: {
		void *pt = s->alloc_point;
		cmAllocationHeader *header = cast(cmAllocationHeader *)pt;
		void *data = cm_align_forward(header+1, alignment);
		void *end = cm_pointer_add(s->physical_start, s->total_size);

		CM_ASSERT(alignment % 4 == 0);
		size = ((size + 3)/4)*4;
		pt = cm_pointer_add(pt, size);

		// NOTE(bill): Wrap around
		if (pt > end) {
			header->size = cm_pointer_diff(header, end) | CM_ISIZE_HIGH_BIT;
			pt = s->physical_start;
			header = cast(cmAllocationHeader *)pt;
			data = cm_align_forward(header+1, alignment);
			pt = cm_pointer_add(pt, size);
		}

		if (!cm_scratch_memory_is_in_use(s, pt)) {
			cm_allocation_header_fill(header, pt, cm_pointer_diff(header, pt));
			s->alloc_point = cast(u8 *)pt;
			ptr = data;
		}

		if (flags & cmAllocatorFlag_ClearToZero)
			cm_zero_size(ptr, size);
	} break;

	case cmAllocation_Free: {
		if (old_memory) {
			void *end = cm_pointer_add(s->physical_start, s->total_size);
			if (old_memory < s->physical_start || old_memory >= end) {
				CM_ASSERT(false);
			} else {
				// NOTE(bill): Mark as free
				cmAllocationHeader *h = cm_allocation_header(old_memory);
				CM_ASSERT((h->size & CM_ISIZE_HIGH_BIT) == 0);
				h->size = h->size | CM_ISIZE_HIGH_BIT;

				while (s->free_point != s->alloc_point) {
					cmAllocationHeader *header = cast(cmAllocationHeader *)s->free_point;
					if ((header->size & CM_ISIZE_HIGH_BIT) == 0)
						break;

					s->free_point = cm_pointer_add(s->free_point, h->size & (~CM_ISIZE_HIGH_BIT));
					if (s->free_point == end)
						s->free_point = s->physical_start;
				}
			}
		}
	} break;

	case cmAllocation_FreeAll:
		s->alloc_point = s->physical_start;
		s->free_point  = s->physical_start;
		break;

	case cmAllocation_Resize:
		ptr = cm_default_resize_align(cm_scratch_allocator(s), old_memory, old_size, size, alignment);
		break;
	}

	return ptr;
}


CM_END_EXTERN