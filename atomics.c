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

#include "atomics.h"
#include "fences.h"
#include "utils.h"

//
// Concurrency
//

// IMPORTANT TODO(bill): Use compiler intrinsics for the atomics

#if defined(CM_COMPILER_MSVC) && !defined(CM_COMPILER_CLANG)
cm_inline i32  
cm_atomic32_load (cmAtomic32 const volatile *a) { 
    return a->value;  
}

cm_inline void 
cm_atomic32_store(cmAtomic32 volatile *a, i32 value) { 
    a->value = value; 
}

cm_inline i32 
cm_atomic32_compare_exchange(cmAtomic32 volatile *a, i32 expected, i32 desired) {
	return _InterlockedCompareExchange(cast(long volatile *)a, desired, expected);
}

cm_inline i32 
cm_atomic32_exchanged(cmAtomic32 volatile *a, i32 desired) {
	return _InterlockedExchange(cast(long volatile *)a, desired);
}

cm_inline i32 
cm_atomic32_fetch_add(cmAtomic32 volatile *a, i32 operand) {
	return _InterlockedExchangeAdd(cast(long volatile *)a, operand);
}

cm_inline i32 
cm_atomic32_fetch_and(cmAtomic32 volatile *a, i32 operand) {
	return _InterlockedAnd(cast(long volatile *)a, operand);
}

cm_inline i32 
cm_atomic32_fetch_or(cmAtomic32 volatile *a, i32 operand) {
	return _InterlockedOr(cast(long volatile *)a, operand);
}

cm_inline i64 
cm_atomic64_load(cmAtomic64 const volatile *a) {
#if defined(CM_ARCH_64)
	return a->value;
#elif CM_CPU_X86
	// NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b
	i64 result;
	__asm {
		mov esi, a;
		mov ebx, eax;
		mov ecx, edx;
		lock cmpxchg8b [esi];
		mov dword ptr result, eax;
		mov dword ptr result[4], edx;
	}
	return result;
#else
#error TODO(bill): atomics for this CPU
#endif
}


cm_inline void 
cm_atomic64_store(cmAtomic64 volatile *a, i64 value) {
#if defined(CM_ARCH_64)
	a->value = value;
#elif CM_CPU_X86
	// NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b
	__asm {
		mov esi, a;
		mov ebx, dword ptr value;
		mov ecx, dword ptr value[4];
	retry:
		cmpxchg8b [esi];
		jne retry;
	}
#else
#error TODO(bill): atomics for this CPU
#endif
}

cm_inline i64 
cm_atomic64_compare_exchange(cmAtomic64 volatile *a, i64 expected, i64 desired) {
	return _InterlockedCompareExchange64(cast(i64 volatile *)a, desired, expected);
}

cm_inline i64 
cm_atomic64_exchanged(cmAtomic64 volatile *a, i64 desired) {
#if defined(CM_ARCH_64)
	return _InterlockedExchange64(cast(i64 volatile *)a, desired);
#elif CM_CPU_X86
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, desired, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#else
#error TODO(bill): atomics for this CPU
#endif
}

cm_inline i64 
cm_atomic64_fetch_add(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	return _InterlockedExchangeAdd64(cast(i64 volatile *)a, operand);
#elif CM_CPU_X86
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected + operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#else
#error TODO(bill): atomics for this CPU
#endif
}

cm_inline i64 
cm_atomic64_fetch_and(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	return _InterlockedAnd64(cast(i64 volatile *)a, operand);
#elif CM_CPU_X86
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected & operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#else
#error TODO(bill): atomics for this CPU
#endif
}

cm_inline i64 
cm_atomic64_fetch_or(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	return _InterlockedOr64(cast(i64 volatile *)a, operand);
#elif CM_CPU_X86
	i64 expected = a->value;
	for (;;) {
		i64 original = _InterlockedCompareExchange64(cast(i64 volatile *)a, expected | operand, expected);
		if (original == expected)
			return original;
		expected = original;
	}
#else
#error TODO(bill): atomics for this CPU
#endif
}



#elif defined(CM_CPU_X86)

cm_inline i32  
cm_atomic32_load (cmAtomic32 const volatile *a) { 
    return a->value;  
}

cm_inline void 
cm_atomic32_store(cmAtomic32 volatile *a, i32 value) { 
    a->value = value; 
}

cm_inline i32 
cm_atomic32_compare_exchange(cmAtomic32 volatile *a, i32 expected, i32 desired) {
	i32 original;
	__asm__ volatile(
		"lock; cmpxchgl %2, %1"
		: "=a"(original), "+m"(a->value)
		: "q"(desired), "0"(expected)
	);
	return original;
}

cm_inline i32 
cm_atomic32_exchanged(cmAtomic32 volatile *a, i32 desired) {
	// NOTE(bill): No lock prefix is necessary for xchgl
	i32 original;
	__asm__ volatile(
		"xchgl %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(desired)
	);
	return original;
}

cm_inline i32 
cm_atomic32_fetch_add(cmAtomic32 volatile *a, i32 operand) {
	i32 original;
	__asm__ volatile(
		"lock; xaddl %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(operand)
	);
	return original;
}

cm_inline i32 
cm_atomic32_fetch_and(cmAtomic32 volatile *a, i32 operand) {
	i32 original;
	i32 tmp;
	__asm__ volatile(
		"1:     movl    %1, %0\n"
		"       movl    %0, %2\n"
		"       andl    %3, %2\n"
		"       lock; cmpxchgl %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(tmp)
		: "r"(operand)
	);
	return original;
}

cm_inline i32 
cm_atomic32_fetch_or(cmAtomic32 volatile *a, i32 operand) {
	i32 original;
	i32 temp;
	__asm__ volatile(
		"1:     movl    %1, %0\n"
		"       movl    %0, %2\n"
		"       orl     %3, %2\n"
		"       lock; cmpxchgl %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(temp)
		: "r"(operand)
	);
	return original;
}

cm_inline i64 
cm_atomic64_load(cmAtomic64 const volatile *a) {
#if defined(CM_ARCH_64)
	return a->value;
#else
	i64 original;
	__asm__ volatile(
		"movl %%ebx, %%eax\n"
		"movl %%ecx, %%edx\n"
		"lock; cmpxchg8b %1"
		: "=&A"(original)
		: "m"(a->value)
	);
	return original;
#endif
}

cm_inline void 
cm_atomic64_store(cmAtomic64 volatile *a, i64 value) {
#if defined(CM_ARCH_64)
	a->value = value;
#else
	i64 expected = a->value;
	__asm__ volatile(
		"1:    cmpxchg8b %0\n"
		"      jne 1b"
		: "=m"(a->value)
		: "b"((i32)value), "c"((i32)(value >> 32)), "A"(expected)
	);
#endif
}

cm_inline i64 
cm_atomic64_compare_exchange(cmAtomic64 volatile *a, i64 expected, i64 desired) {
#if defined(CM_ARCH_64)
	i64 original;
	__asm__ volatile(
		"lock; cmpxchgq %2, %1"
		: "=a"(original), "+m"(a->value)
		: "q"(desired), "0"(expected)
	);
	return original;
#else
	i64 original;
	__asm__ volatile(
		"lock; cmpxchg8b %1"
		: "=A"(original), "+m"(a->value)
		: "b"((i32)desired), "c"((i32)(desired >> 32)), "0"(expected)
	);
	return original;
#endif
}

cm_inline i64 
cm_atomic64_exchanged(cmAtomic64 volatile *a, i64 desired) {
#if defined(CM_ARCH_64)
	i64 original;
	__asm__ volatile(
		"xchgq %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(desired)
	);
	return original;
#else
	i64 original = a->value;
	for (;;) {
		i64 previous = cm_atomic64_compare_exchange(a, original, desired);
		if (original == previous)
			return original;
		original = previous;
	}
#endif
}

cm_inline i64 
cm_atomic64_fetch_add(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	i64 original;
	__asm__ volatile(
		"lock; xaddq %0, %1"
		: "=r"(original), "+m"(a->value)
		: "0"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (cm_atomic64_compare_exchange(a, original, original + operand) == original)
			return original;
	}
#endif
}

cm_inline i64 
cm_atomic64_fetch_and(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	i64 original;
	i64 tmp;
	__asm__ volatile(
		"1:     movq    %1, %0\n"
		"       movq    %0, %2\n"
		"       andq    %3, %2\n"
		"       lock; cmpxchgq %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(tmp)
		: "r"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (cm_atomic64_compare_exchange(a, original, original & operand) == original)
			return original;
	}
#endif
}

cm_inline i64 
cm_atomic64_fetch_or(cmAtomic64 volatile *a, i64 operand) {
#if defined(CM_ARCH_64)
	i64 original;
	i64 temp;
	__asm__ volatile(
		"1:     movq    %1, %0\n"
		"       movq    %0, %2\n"
		"       orq     %3, %2\n"
		"       lock; cmpxchgq %2, %1\n"
		"       jne     1b"
		: "=&a"(original), "+m"(a->value), "=&r"(temp)
		: "r"(operand)
	);
	return original;
#else
	for (;;) {
		i64 original = a->value;
		if (cm_atomic64_compare_exchange(a, original, original | operand) == original)
			return original;
	}
#endif
}

#else
#error TODO(bill): Implement Atomics for this CPU
#endif

cm_inline b32 
cm_atomic32_spin_lock(cmAtomic32 volatile *a, isize time_out) {
	i32 old_value = cm_atomic32_compare_exchange(a, 1, 0);
	i32 counter = 0;
	while (old_value != 0 && (time_out < 0 || counter++ < time_out)) {
		cm_yield_thread();
		old_value = cm_atomic32_compare_exchange(a, 1, 0);
		cm_mfence();
	}
	return old_value == 0;
}

cm_inline void 
cm_atomic32_spin_unlock(cmAtomic32 volatile *a) {
	cm_atomic32_store(a, 0);
	cm_mfence();
}

cm_inline b32 
cm_atomic64_spin_lock(cmAtomic64 volatile *a, isize time_out) {
	i64 old_value = cm_atomic64_compare_exchange(a, 1, 0);
	i64 counter = 0;
	while (old_value != 0 && (time_out < 0 || counter++ < time_out)) {
		cm_yield_thread();
		old_value = cm_atomic64_compare_exchange(a, 1, 0);
		cm_mfence();
	}
	return old_value == 0;
}

cm_inline void 
cm_atomic64_spin_unlock(cmAtomic64 volatile *a) {
	cm_atomic64_store(a, 0);
	cm_mfence();
}

cm_inline b32 
cm_atomic32_try_acquire_lock(cmAtomic32 volatile *a) {
	i32 old_value;
	cm_yield_thread();
	old_value = cm_atomic32_compare_exchange(a, 1, 0);
	cm_mfence();
	return old_value == 0;
}

cm_inline b32 
cm_atomic64_try_acquire_lock(cmAtomic64 volatile *a) {
	i64 old_value;
	cm_yield_thread();
	old_value = cm_atomic64_compare_exchange(a, 1, 0);
	cm_mfence();
	return old_value == 0;
}


#if defined(CM_ARCH_32)

cm_inline void *
cm_atomic_ptr_load(cmAtomicPtr const volatile *a) {
	return cast(void *)cast(intptr)cm_atomic32_load(cast(cmAtomic32 const volatile *)a);
}

cm_inline void 
cm_atomic_ptr_store(cmAtomicPtr volatile *a, void *value) {
	cm_atomic32_store(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)value);
}

cm_inline void *
cm_atomic_ptr_compare_exchange(cmAtomicPtr volatile *a, void *expected, void *desired) {
	return cast(void *)cast(intptr)cm_atomic32_compare_exchange(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)expected, cast(i32)cast(intptr)desired);
}

cm_inline void *
cm_atomic_ptr_exchanged(cmAtomicPtr volatile *a, void *desired) {
	return cast(void *)cast(intptr)cm_atomic32_exchanged(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)desired);
}

cm_inline void *
cm_atomic_ptr_fetch_add(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic32_fetch_add(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)operand);
}

cm_inline void *
cm_atomic_ptr_fetch_and(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic32_fetch_and(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)operand);
}

cm_inline void *
cm_atomic_ptr_fetch_or(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic32_fetch_or(cast(cmAtomic32 volatile *)a, cast(i32)cast(intptr)operand);
}

cm_inline b32 
cm_atomic_ptr_spin_lock(cmAtomicPtr volatile *a, isize time_out) {
	return cm_atomic32_spin_lock(cast(cmAtomic32 volatile *)a, time_out);
}

cm_inline void 
cm_atomic_ptr_spin_unlock(cmAtomicPtr volatile *a) {
	cm_atomic32_spin_unlock(cast(cmAtomic32 volatile *)a);
}

cm_inline b32 
cm_atomic_ptr_try_acquire_lock(cmAtomicPtr volatile *a) {
	return cm_atomic32_try_acquire_lock(cast(cmAtomic32 volatile *)a);
}

#elif defined(CM_ARCH_64)

cm_inline void *
cm_atomic_ptr_load(cmAtomicPtr const volatile *a) {
	return cast(void *)cast(intptr)cm_atomic64_load(cast(cmAtomic64 const volatile *)a);
}

cm_inline void 
cm_atomic_ptr_store(cmAtomicPtr volatile *a, void *value) {
	cm_atomic64_store(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)value);
}

cm_inline void *
cm_atomic_ptr_compare_exchange(cmAtomicPtr volatile *a, void *expected, void *desired) {
	return cast(void *)cast(intptr)cm_atomic64_compare_exchange(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)expected, cast(i64)cast(intptr)desired);
}

cm_inline void *
cm_atomic_ptr_exchanged(cmAtomicPtr volatile *a, void *desired) {
	return cast(void *)cast(intptr)cm_atomic64_exchanged(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)desired);
}

cm_inline void *
cm_atomic_ptr_fetch_add(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic64_fetch_add(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)operand);
}

cm_inline void *
cm_atomic_ptr_fetch_and(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic64_fetch_and(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)operand);
}

cm_inline void *
cm_atomic_ptr_fetch_or(cmAtomicPtr volatile *a, void *operand) {
	return cast(void *)cast(intptr)cm_atomic64_fetch_or(cast(cmAtomic64 volatile *)a, cast(i64)cast(intptr)operand);
}
cm_inline b32 
cm_atomic_ptr_spin_lock(cmAtomicPtr volatile *a, isize time_out) {
	return cm_atomic64_spin_lock(cast(cmAtomic64 volatile *)a, time_out);
}

cm_inline void 
cm_atomic_ptr_spin_unlock(cmAtomicPtr volatile *a) {
	cm_atomic64_spin_unlock(cast(cmAtomic64 volatile *)a);
}

cm_inline b32 
cm_atomic_ptr_try_acquire_lock(cmAtomicPtr volatile *a) {
	return cm_atomic64_try_acquire_lock(cast(cmAtomic64 volatile *)a);
}
#endif