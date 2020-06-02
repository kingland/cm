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

// TODO(bill): Is this enough to get inline working?
#if !defined(__cplusplus)
	#if defined(_MSC_VER) && _MSC_VER <= 1800
	#define inline __inline
	#elif !defined(__STDC_VERSION__)
	#define inline __inline__
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