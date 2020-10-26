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

#include "sortsearch.h"
#include "memory.h"
#include "random.h"
#include "char.h"

/*
//
// Sorting
//
*/

// TODO(bill): Should I make all the macros local?

#define CM__COMPARE_PROC(Type) \
cm_global isize cm__##Type##_cmp_offset; \
CM_COMPARE_PROC(cm__##Type##_cmp) { \
	Type const p = *cast(Type const *)cm_pointer_add_const(a, cm__##Type##_cmp_offset); \
	Type const q = *cast(Type const *)cm_pointer_add_const(b, cm__##Type##_cmp_offset); \
	return p < q ? -1 : p > q; \
} \
CM_COMPARE_PROC_PTR(cm_##Type##_cmp(isize offset)) { \
	cm__##Type##_cmp_offset = offset; \
	return &cm__##Type##_cmp; \
}

CM__COMPARE_PROC(i16);
CM__COMPARE_PROC(i32);
CM__COMPARE_PROC(i64);
CM__COMPARE_PROC(isize);
CM__COMPARE_PROC(f32);
CM__COMPARE_PROC(f64);
CM__COMPARE_PROC(char);

// NOTE(bill): str_cmp is special as it requires a funny type and funny comparison
cm_global isize cm__str_cmp_offset; 

CM_COMPARE_PROC(cm__str_cmp) {
	char const *p = *cast(char const **)cm_pointer_add_const(a, cm__str_cmp_offset);
	char const *q = *cast(char const **)cm_pointer_add_const(b, cm__str_cmp_offset);
	return cm_strcmp(p, q);
}

CM_COMPARE_PROC_PTR(cm_str_cmp(isize offset)) {
	cm__str_cmp_offset = offset;
	return &cm__str_cmp;
}

#undef CM__COMPARE_PROC

// TODO(bill): Make user definable?
#define CM__SORT_STACK_SIZE            64
#define CM__SORT_INSERT_SORT_THRESHOLD  8

#define CM__SORT_PUSH(_base, _limit) do { \
	stack_ptr[0] = (_base); \
	stack_ptr[1] = (_limit); \
	stack_ptr += 2; \
} while (0)


#define CM__SORT_POP(_base, _limit) do { \
	stack_ptr -= 2; \
	(_base)  = stack_ptr[0]; \
	(_limit) = stack_ptr[1]; \
} while (0)

void 
cm_sort(void *base_, isize count, isize size, cmCompareProc cmp) {
	u8 *i, *j;
	u8 *base = cast(u8 *)base_;
	u8 *limit = base + count*size;
	isize threshold = CM__SORT_INSERT_SORT_THRESHOLD * size;

	// NOTE(bill): Prepare the stack
	u8 *stack[CM__SORT_STACK_SIZE] = {0};
	u8 **stack_ptr = stack;

	for (;;) {
		if ((limit-base) > threshold) {
			// NOTE(bill): Quick sort
			i = base + size;
			j = limit - size;

			cm_memswap(((limit-base)/size/2) * size + base, base, size);
			if (cmp(i, j) > 0)    cm_memswap(i, j, size);
			if (cmp(base, j) > 0) cm_memswap(base, j, size);
			if (cmp(i, base) > 0) cm_memswap(i, base, size);

			for (;;) {
				do i += size; while (cmp(i, base) < 0);
				do j -= size; while (cmp(j, base) > 0);
				if (i > j) break;
				cm_memswap(i, j, size);
			}

			cm_memswap(base, j, size);

			if (j - base > limit - i) {
				CM__SORT_PUSH(base, j);
				base = i;
			} else {
				CM__SORT_PUSH(i, limit);
				limit = j;
			}
		} else {
			// NOTE(bill): Insertion sort
			for (j = base, i = j+size;
			     i < limit;
			     j = i, i += size) {
				for (; cmp(j, j+size) > 0; j -= size) {
					cm_memswap(j, j+size, size);
					if (j == base) break;
				}
			}

			if (stack_ptr == stack) break; // NOTE(bill): Sorting is done!
			CM__SORT_POP(base, limit);
		}
	}
}


#undef CM__SORT_PUSH
#undef CM__SORT_POP

#define CM_RADIX_SORT_PROC_GEN(Type) CM_RADIX_SORT_PROC(Type) { \
	Type *source = items; \
	Type *dest   = temp; \
	isize byte_index, i, byte_max = 8*cm_size_of(Type); \
	for (byte_index = 0; byte_index < byte_max; byte_index += 8) { \
		isize offsets[256] = {0}; \
		isize total = 0; \
		/* NOTE(bill): First pass - count how many of each key */ \
		for (i = 0; i < count; i++) { \
			Type radix_value = source[i]; \
			Type radix_piece = (radix_value >> byte_index) & 0xff; \
			offsets[radix_piece]++; \
		} \
		/* NOTE(bill): Change counts to offsets */ \
		for (i = 0; i < cm_count_of(offsets); i++) { \
			isize skcount = offsets[i]; \
			offsets[i] = total; \
			total += skcount; \
		} \
		/* NOTE(bill): Second pass - place elements into the right location */ \
		for (i = 0; i < count; i++) { \
			Type radix_value = source[i]; \
			Type radix_piece = (radix_value >> byte_index) & 0xff; \
			dest[offsets[radix_piece]++] = source[i]; \
		} \
		cm_swap(Type *, source, dest); \
	} \
}

CM_RADIX_SORT_PROC_GEN(u8);
CM_RADIX_SORT_PROC_GEN(u16);
CM_RADIX_SORT_PROC_GEN(u32);
CM_RADIX_SORT_PROC_GEN(u64);

cm_inline isize 
cm_binary_search(void const *base, isize count, isize size, void const *key, cmCompareProc compare_proc) {
	isize start = 0;
	isize end = count;

	while (start < end) {
		isize mid = start + (end-start)/2;
		isize result = compare_proc(key, cast(u8 *)base + mid*size);
		if (result < 0)
			end = mid;
		else if (result > 0)
			start = mid+1;
		else
			return mid;
	}

	return -1;
}

void 
cm_shuffle(void *base, isize count, isize size) {
	u8 *a;
	isize i, j;
	cmRandom random; cm_random_init(&random);

	a = cast(u8 *)base + (count-1) * size;
	for (i = count; i > 1; i--) {
		j = cm_random_gen_isize(&random) % i;
		cm_memswap(a, cast(u8 *)base + j*size, size);
		a -= size;
	}
}

void 
cm_reverse(void *base, isize count, isize size) {
	isize i, j = count-1;
	for (i = 0; i < j; i++, j++) {
		cm_memswap(cast(u8 *)base + i*size, cast(u8 *)base + j*size, size);
	}
}