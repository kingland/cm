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

#ifndef CM_ATOMICS_H
#define CM_ATOMICS_H

#include "dll.h"
#include "arch.h"
#include "types.h"

// TODO(bill): Be specific with memory order?
// e.g. relaxed, acquire, release, acquire_release

#if defined(CM_COMPILER_MSVC)
typedef struct cmAtomic32  { i32   volatile value; } cmAtomic32;
typedef struct cmAtomic64  { i64   volatile value; } cmAtomic64;
typedef struct cmAtomicPtr { void *volatile value; } cmAtomicPtr;
#else
	#if defined(CM_ARCH_32)
	#define CM_ATOMIC_PTR_ALIGNMENT 4
	#elif defined(CM_ARCH_64)
	#define CM_ATOMIC_PTR_ALIGNMENT 8
	#else
	#error Unknown architecture
	#endif

typedef struct cmAtomic32  { i32   volatile value; } __attribute__ ((aligned(4))) cmAtomic32;
typedef struct cmAtomic64  { i64   volatile value; } __attribute__ ((aligned(8))) cmAtomic64;
typedef struct cmAtomicPtr { void *volatile value; } __attribute__ ((aligned(CM_ATOMIC_PTR_ALIGNMENT))) cmAtomicPtr;
#endif

CM_DEF i32  cm_atomic32_load               (cmAtomic32 const volatile *a);
CM_DEF void cm_atomic32_store              (cmAtomic32 volatile *a, i32 value);
CM_DEF i32  cm_atomic32_compare_exchange   (cmAtomic32 volatile *a, i32 expected, i32 desired);
CM_DEF i32  cm_atomic32_exchanged          (cmAtomic32 volatile *a, i32 desired);
CM_DEF i32  cm_atomic32_fetch_add          (cmAtomic32 volatile *a, i32 operand);
CM_DEF i32  cm_atomic32_fetch_and          (cmAtomic32 volatile *a, i32 operand);
CM_DEF i32  cm_atomic32_fetch_or           (cmAtomic32 volatile *a, i32 operand);
CM_DEF b32  cm_atomic32_spin_lock          (cmAtomic32 volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
CM_DEF void cm_atomic32_spin_unlock        (cmAtomic32 volatile *a);
CM_DEF b32  cm_atomic32_try_acquire_lock   (cmAtomic32 volatile *a);

CM_DEF i64  cm_atomic64_load               (cmAtomic64 const volatile *a);
CM_DEF void cm_atomic64_store              (cmAtomic64 volatile *a, i64 value);
CM_DEF i64  cm_atomic64_compare_exchange   (cmAtomic64 volatile *a, i64 expected, i64 desired);
CM_DEF i64  cm_atomic64_exchanged          (cmAtomic64 volatile *a, i64 desired);
CM_DEF i64  cm_atomic64_fetch_add          (cmAtomic64 volatile *a, i64 operand);
CM_DEF i64  cm_atomic64_fetch_and          (cmAtomic64 volatile *a, i64 operand);
CM_DEF i64  cm_atomic64_fetch_or           (cmAtomic64 volatile *a, i64 operand);
CM_DEF b32  cm_atomic64_spin_lock          (cmAtomic64 volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
CM_DEF void cm_atomic64_spin_unlock        (cmAtomic64 volatile *a);
CM_DEF b32  cm_atomic64_try_acquire_lock   (cmAtomic64 volatile *a);


CM_DEF void *cm_atomic_ptr_load            (cmAtomicPtr const volatile *a);
CM_DEF void  cm_atomic_ptr_store           (cmAtomicPtr volatile *a, void *value);
CM_DEF void *cm_atomic_ptr_compare_exchange(cmAtomicPtr volatile *a, void *expected, void *desired);
CM_DEF void *cm_atomic_ptr_exchanged       (cmAtomicPtr volatile *a, void *desired);
CM_DEF void *cm_atomic_ptr_fetch_add       (cmAtomicPtr volatile *a, void *operand);
CM_DEF void *cm_atomic_ptr_fetch_and       (cmAtomicPtr volatile *a, void *operand);
CM_DEF void *cm_atomic_ptr_fetch_or        (cmAtomicPtr volatile *a, void *operand);
CM_DEF b32   cm_atomic_ptr_spin_lock       (cmAtomicPtr volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
CM_DEF void  cm_atomic_ptr_spin_unlock     (cmAtomicPtr volatile *a);
CM_DEF b32   cm_atomic_ptr_try_acquire_lock(cmAtomicPtr volatile *a);

#endif //CM_ATOMICS_H
