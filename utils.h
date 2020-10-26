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

#include "pragma.h"

#ifndef CM_UTILS_H
#define CM_UTILS_H

#ifndef NULL
	#if defined(__cplusplus)
		#if __cplusplus >= 201103L
			#define NULL nullptr
		#else
			#define NULL 0
		#endif
	#else
		#define NULL ((void *)0)
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//	https://stackoverflow.com/questions/2765164/inline-vs-inline-vs-inline-vs-forceinline
//
//	What are the differences between these four inline (key)words?
//
//	[inline] 
//		is the keyword, in C++ and C99.
//
//	[__inline] 
//		is a vendor-specific keyword (e.g. MSVC) for inline function in C, since C89 doesn't have it.
//
//	[__inline__] 
//		is similar to [__inline] but is from another set of compilers.
//
//	[__forceinline] 
//		is another vendor-specific (mainly MSVC) keyword, 
//		which will apply more force to inline the function than the __inline hint 
//		(e.g. inline even if it result in worse code).
//		There's also __attribute__((always_inline)) in GCC and clang.
//
/////////////////////////////////////////////////////////////////////////////////
//
//	https://en.wikipedia.org/wiki/Inline_function
//
//	Standard support
//	C++ and C99, but not its predecessors K&R C and C89, have support for inline functions, 
//	though with different semantics. In both cases, inline does not force inlining; 
//	the compiler is free to choose not to inline the function at all, or only in some cases. 
//	Different compilers vary in how complex a function they can manage to inline. 
//	Mainstream C++ compilers like Microsoft Visual C++ and GCC support an option that lets the compilers 
//	automatically inline any suitable function, even those not marked as inline functions. 
//	However, simply omitting the inline keyword to let the compiler make all inlining decisions is not possible, 
//	since the linker will then complain about duplicate definitions in different translation units. 
//	This is because inline not only gives the compiler a hint that the function should be inlined, 
//	it also has an effect on whether the compiler will generate a callable out-of-line copy of the function 
//	(see storage classes of inline functions).
//
//	Nonstandard extensions
//	GNU C, as part of the dialect gnu89 that it offers, has support for inline as an extension to C89. 
//	However, the semantics differ from both those of C++ and C99. 
//	armcc in C90 mode also offers inline as a non-standard extension, with semantics different from gnu89 and C99.
//
//	Some implementations provide a means by which to force the compiler to inline a function, 
//	usually by means of implementation-specific declaration specifiers:
//
//	Microsoft Visual C++: __forceinline
//	gcc or clang: __attribute__((always_inline)) or __attribute__((__always_inline__)), 
//	the latter of which is useful to avoid a conflict with a user-defined macro named always_inline.
//
//	Indiscriminate uses of that can result in larger code (bloated executable file), 
//	minimal or no performance gain, and in some cases even a loss in performance. 
//	Moreover, the compiler cannot inline the function in all circumstances, even when inlining is forced; 
//	in this case both gcc and Visual C++ generate warnings.
//
//	Forcing inlining is useful if
//
//	inline is not respected by the compiler (ignored by compiler cost/benefit analyzer), and
//	inlining results in a necessary performance boost
//
/////////////////////////////////////////////////////////////////////////////////

// TODO(bill): Is this enough to get inline working?
#if !defined(__cplusplus)
	/*#if defined(_MSC_VER) && _MSC_VER <= 1800
	#define inline __inline
	#elif !defined(__STDC_VERSION__)
	#define inline __inline__
	#else
	#define inline
	#endif*/
	#if defined(_MSC_VER)
		#define inline __inline
	#elif defined(__GNUC__)
		#if defined(__STRICT_ANSI__)
			#define inline __inline__
		#else
			#define inline inline
		#endif
	#else
    	#define inline
	#endif

#endif

#if !defined(cm_restrict)
	#if defined(_MSC_VER)
		#define cm_restrict __restrict
	#elif defined(__STDC_VERSION__)
		#define cm_restrict restrict
	#else
		#define cm_restrict
	#endif
#endif

// TODO(bill): Should force inline be a separate keyword and cm_inline be inline?
#if !defined(cm_inline)
	#if defined(_MSC_VER)
		#if _MSC_VER < 1300
		#define cm_inline
		#else
		#define cm_inline __forceinline
		#endif
	#else
		#define cm_inline __attribute__ ((__always_inline__))
	#endif
#endif

#if !defined(cm_no_inline)
	#if defined(_MSC_VER)
		#define cm_no_inline __declspec(noinline)
	#else
		#define cm_no_inline __attribute__ ((noinline))
	#endif
#endif

#if !defined(cm_thread_local)
	#if defined(_MSC_VER) && _MSC_VER >= 1300
		#define cm_thread_local __declspec(thread)
	#elif defined(__GNUC__)
		#define cm_thread_local __thread
	#else
		#define cm_thread_local thread_local
	#endif
#endif

// NOTE(bill): Easy to grep
// NOTE(bill): Not needed in macros
#ifndef cast
#define cast(Type) (Type)
#endif

// NOTE(bill): Because a signed sizeof is more useful
#ifndef cm_size_of
#define cm_size_of(x) (isize)(sizeof(x))
#endif

#ifndef cm_count_of
#define cm_count_of(x) ((cm_size_of(x)/cm_size_of(0[x])) / ((isize)(!(cm_size_of(x) % cm_size_of(0[x])))))
#endif

#ifndef cm_offset_of
#define cm_offset_of(Type, element) ((isize)&(((Type *)0)->element))
#endif

#if defined(__cplusplus)
#ifndef cm_align_of
	#if __cplusplus >= 201103L
		#define cm_align_of(Type) (isize)alignof(Type)
	#else
	extern "C++" {
		// NOTE(bill): Fucking Templates!
		template <typename T> 
		struct cmAlignment_Trick { char c; T member; };

		#define cm_align_of(Type) cm_offset_of(cmAlignment_Trick<Type>, member)
	}
	#endif
#endif
#else
	#ifndef cm_align_of
	#define cm_align_of(Type) cm_offset_of(struct { char c; Type member; }, member)
	#endif
#endif

// NOTE(bill): I do wish I had a type_of that was portable
#ifndef cm_swap
#define cm_swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#endif

// NOTE(bill): Because static means 3/4 different things in C/C++. Great design (!)
#ifndef cm_global
#define cm_global        static // Global variables
#define cm_internal      static // Internal linkage
#define cm_local_persist static // Local Persisting variables
#endif


#ifndef cm_unused
	#if defined(_MSC_VER)
		#define cm_unused(x) (__pragma(warning(suppress:4100))(x))
	#elif defined (__GCC__)
		#define cm_unused(x) __attribute__((__unused__))(x)
	#else
		#define cm_unused(x) ((void)(cm_size_of(x)))
	#endif
#endif

#ifdef __cplusplus
// cm_cast converts void* to T*.
// e.g. #define malloc(x) (g_cast (malloc (x)))
// FIXME It used to do more. Rename?
struct vprtcast
{
private:
	void * const x;
public:
	explicit vprtcast (void volatile *y) : x((void*)y) { }
	// Lack of rvalue constructor inhibits ternary operator.
	// Either don't use ternary, or cast each side.
	// sa = (salen <= 128) ? g_alloca (salen) : g_malloc (salen);
	// w32socket.c:1045:24: error: call to deleted constructor of 'monoeg_g_cast'
	//g_cast (g_cast&& y) : x(y.x) { }
	vprtcast (vprtcast&&) = delete;
	vprtcast () = delete;
	vprtcast (const vprtcast&) = delete;

	template <typename TO>
	operator TO* () const
	{
		return (TO*)x;
	}
};
#else
// FIXME? Parens are omitted to preserve prior meaning.
#define voidcast(x) x

#endif

// Join Macros
#ifndef CM_JOIN_MACROS
#define CM_JOIN_MACROS
	#define CM_JOIN2_IND(a, b) a##b

	#define CM_JOIN2(a, b)       CM_JOIN2_IND(a, b)
	#define CM_JOIN3(a, b, c)    CM_JOIN2(CM_JOIN2(a, b), c)
	#define CM_JOIN4(a, b, c, d) CM_JOIN2(CM_JOIN2(CM_JOIN2(a, b), c), d)
#endif

#ifndef CM_BIT
#define CM_BIT(x) (1<<(x))
#endif

// NOTE:
// CM_EXPLODE_32BIT(0xff) => 0xffffffff
// CM_EXPLODE_32BIT(0x80) => 0x80808080
// CM_EXPLODE_32BIT(0x7f) => 0x7f7f7f7f
// CM_EXPLODE_32BIT(0) => 0
#ifndef CM_EXPLODE_32BIT
#define CM_EXPLODE_32BIT(x) (x << 24 | x << 16 | x << 8 | x) 
#endif

#ifndef CM_MIN
#define CM_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef CM_MAX
#define CM_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef CM_MIN3
#define CM_MIN3(a, b, c) CM_MIN(CM_MIN(a, b), c)
#endif

#ifndef CM_MAX3
#define CM_MAX3(a, b, c) CM_MAX(CM_MAX(a, b), c)
#endif

#ifndef CM_CLAMP
#define CM_CLAMP(x, lower, upper) CM_MIN(CM_MAX((x), (lower)), (upper))
#endif

#ifndef CM_CLAMP01
#define CM_CLAMP01(x) CM_CLAMP((x), 0, 1)
#endif

#ifndef CM_IS_BETWEEN
#define CM_IS_BETWEEN(x, lower, upper) (((lower) <= (x)) && ((x) <= (upper)))
#endif

#ifndef CM_ABS
#define CM_ABS(x) ((x) < 0 ? -(x) : (x))
#endif

/* NOTE(bill): Very useful bit setting */
#ifndef CM_MASK_SET
#define CM_MASK_SET(var, set, mask) do { \
	if (set) (var) |=  (mask); \
	else     (var) &= ~(mask); \
} while (0)
#endif

// NOTE(bill): Some compilers support applying printf-style warnings to user functions.
#if defined(__clang__) || defined(__GNUC__)
#define CM_PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define CM_PRINTF_ARGS(FMT)
#endif

/*
 * give a hint to the compiler which branch is more likely or unlikely
 * to occur, e.g. rare error cases:
 *
 * if (CM_UNLIKELY(failure == 0))
 *    return NULL;
 *
 * the double !! is to cast the expression (e.g. NULL) to a boolean required by
 * the intrinsic
 */
#ifdef defined(__clang__) || defined(__GNUC__)
#define CM_LIKELY(x) __builtin_expect(!!(x), 1)
#define CM_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CM_LIKELY(x) (x)
#define CN_UNLIKELY(x) (x)
#endif

#ifndef cm_kilobytes
#define cm_kilobytes(x) (            (x) * (i64)(1024))
#endif
#ifndef cm_megabytes
#define cm_megabytes(x) (cm_kilobytes(x) * (i64)(1024))
#endif
#ifndef cm_gigabytes
#define cm_gigabytes(x) (cm_megabytes(x) * (i64)(1024))
#endif
#ifndef cm_terabytes
#define cm_terabytes(x) (cm_gigabytes(x) * (i64)(1024))
#endif

#endif //CM_UTILS_H