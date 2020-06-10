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

#ifndef CM_TYPE_H
#define CM_TYPE_H

#include "arch.h"
#include "assert.h"
#include "header.h"
#include "utils.h"

#if defined(CM_COMPILER_MSVC) || defined(CM_MSVC)
	#if _MSC_VER < 1300
	typedef unsigned char     u8;
	typedef   signed char     i8;
	typedef unsigned short   u16;
	typedef   signed short   i16;
	typedef unsigned int     u32;
	typedef   signed int     i32;
	#else
	typedef unsigned __int8   u8;
	typedef   signed __int8   i8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 i16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 i32;
	#endif
	typedef unsigned __int64 u64;
	typedef   signed __int64 i64;
#else
	#include <stdint.h>
	typedef uint8_t   u8;
	typedef  int8_t   i8;
	typedef uint16_t u16;
	typedef  int16_t i16;
	typedef uint32_t u32;
	typedef  int32_t i32;
	typedef uint64_t u64;
	typedef  int64_t i64;
#endif

CM_STATIC_ASSERT(sizeof(u8)  == sizeof(i8));
CM_STATIC_ASSERT(sizeof(u16) == sizeof(i16));
CM_STATIC_ASSERT(sizeof(u32) == sizeof(i32));
CM_STATIC_ASSERT(sizeof(u64) == sizeof(i64));

CM_STATIC_ASSERT(sizeof(u8)  == 1);
CM_STATIC_ASSERT(sizeof(u16) == 2);
CM_STATIC_ASSERT(sizeof(u32) == 4);
CM_STATIC_ASSERT(sizeof(u64) == 8);

//NOTE: Predefine Pointer-Related Type
//1. size_t 
//	Provide safe type for sizes 
//  The type size_t represents the maximum size any object can be in C. It puspose is to provide a portable means of 
//	declaring a size consistent with addressable area of memory available on system.
//	Usaully size_t can be used to store pointer , but it is not good idea to assume size_t it the same size as a pointer,
//	intprt_t is better choice

//2. ptrdiff_t 
//	Handle pointer arithmetic
//	The type ptrdiff_t is portable way to express the difference two pointer. The result of subtracing two pointer 
//	returned as a ptrdiff_t type
//	
//3. (u)intprt_t 
//	Storing pointer addresss
//	The type (u)intptr_t used for storing pointer addresses. They provide a portable and safe way of declaring pointers
//	char c;
//	uintptr_t *pc = (uintptr_t *)&c;
//	int n;
//	intptr_t *pi = (intptr_t *)&n;

typedef size_t    usize;
typedef ptrdiff_t isize;

CM_STATIC_ASSERT(sizeof(usize) == sizeof(isize));

// NOTE(bill): (u)intptr is only here for semantic reasons really as this library will only support 32/64 bit OSes.
// NOTE(bill): Are there any modern OSes (not 16 bit) where intptr != isize ?
#if defined(_WIN64)
	typedef signed   __int64  intptr;
	typedef unsigned __int64 uintptr;
#elif defined(_WIN32)
	// NOTE(bill); To mark types changing their size, e.g. intptr
	#ifndef _W64
		#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
			#define _W64 __w64
		#else
			#define _W64
		#endif
	#endif

	typedef _W64   signed int  intptr;
	typedef _W64 unsigned int uintptr;
#else
	typedef uintptr_t uintptr;
	typedef  intptr_t  intptr;
#endif

CM_STATIC_ASSERT(sizeof(uintptr) == sizeof(intptr));

typedef float  f32;
typedef double f64;

CM_STATIC_ASSERT(sizeof(f32) == 4);
CM_STATIC_ASSERT(sizeof(f64) == 8);

typedef i32 Rune; // NOTE(bill): Unicode codepoint
#define CM_RUNE_INVALID cast(Rune)(0xfffd)
#define CM_RUNE_MAX     cast(Rune)(0x0010ffff)
#define CM_RUNE_BOM     cast(Rune)(0xfeff)
#define CM_RUNE_EOF     cast(Rune)(-1)

typedef i8  b8;
typedef i16 b16;
typedef i32 b32; // NOTE(bill): Prefer this!!!

// NOTE(bill): Get true and false
#if !defined(__cplusplus)
	#if (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
		#ifndef true
		#define true  (0 == 0)
		#endif
		#ifndef false
		#define false (0 != 0)
		#endif
		typedef b8 bool;
	#else
		#include <stdbool.h>
	#endif
#endif

// NOTE(bill): These do are not prefixed with gb because the types are not.
#ifndef U8_MIN
#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#if defined(CM_ARCH_32)
	#define USIZE_MIX U32_MIN
	#define USIZE_MAX U32_MAX

	#define ISIZE_MIX S32_MIN
	#define ISIZE_MAX S32_MAX
#elif defined(CM_ARCH_64)
	#define USIZE_MIX U64_MIN
	#define USIZE_MAX U64_MAX

	#define ISIZE_MIX I64_MIN
	#define ISIZE_MAX I64_MAX
#else
	#error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#endif

#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f

#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308

#endif

#endif //CM_TYPE_H