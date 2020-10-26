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

#include "time.h"
#include "utils.h"
#include "header.h"
#include "debug.h"

//
// Time
//

#if defined(CM_COMPILER_MSVC) && !defined(__clang__)
	cm_inline u64 cm_rdtsc(void) { return __rdtsc(); }
#elif defined(__i386__)
	cm_inline u64 cm_rdtsc(void) {
		u64 x;
		__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
		return x;
	}
#elif defined(__x86_64__)
	cm_inline u64 cm_rdtsc(void) {
		u32 hi, lo;
		__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
		return (cast(u64)lo) | ((cast(u64)hi)<<32);
	}
#elif defined(__powerpc__)
	cm_inline u64 cm_rdtsc(void) {
		u64 result = 0;
		u32 upper, lower,tmp;
		__asm__ volatile(
			"0:                   \n"
			"\tmftbu   %0         \n"
			"\tmftb    %1         \n"
			"\tmftbu   %2         \n"
			"\tcmpw    %2,%0      \n"
			"\tbne     0b         \n"
			: "=r"(upper),"=r"(lower),"=r"(tmp)
		);
		result = upper;
		result = result<<32;
		result = result|lower;

		return result;
	}
#endif

#if defined(CM_SYS_WINDOWS)

	cm_inline f64 cm_time_now(void) {
		cm_local_persist LARGE_INTEGER win32_perf_count_freq = {0};
		f64 result;
		LARGE_INTEGER counter;
		if (!win32_perf_count_freq.QuadPart) {
			QueryPerformanceFrequency(&win32_perf_count_freq);
			CM_ASSERT(win32_perf_count_freq.QuadPart != 0);
		}

		QueryPerformanceCounter(&counter);

		result = counter.QuadPart / cast(f64)(win32_perf_count_freq.QuadPart);
		return result;
	}

	cm_inline u64 cm_utc_time_now(void) {
		FILETIME ft;
		ULARGE_INTEGER li;

		GetSystemTimeAsFileTime(&ft);
		li.LowPart = ft.dwLowDateTime;
		li.HighPart = ft.dwHighDateTime;

		return li.QuadPart/10;
	}

	cm_inline void cm_sleep_ms(u32 ms) { Sleep(ms); }

#else

	cm_global f64 cm__timebase  = 0.0;
	cm_global u64 cm__timestart = 0;

	cm_inline f64 
	cm_time_now(void) {
#if defined(CM_SYS_OSX)
		f64 result;

		if (!cm__timestart) {
			mach_timebase_info_data_t tb = {0};
			mach_timebase_info(&tb);
			cm__timebase = tb.numer;
			cm__timebase /= tb.denom;
			cm__timestart = mach_absolute_time();
		}

		// NOTE(bill): mach_absolute_time() returns things in nanoseconds
		result = 1.0e-9 * (mach_absolute_time() - cm__timestart) * cm__timebase;
		return result;
#else
		struct timespec t;
		f64 result;

		// IMPORTANT TODO(bill): THIS IS A HACK
		clock_gettime(1 /*CLOCK_MONOTONIC*/, &t);
		result = t.tv_sec + 1.0e-9 * t.tv_nsec;
		return result;
#endif
	}

	cm_inline 
	u64 cm_utc_time_now(void) {
		struct timespec t;
#if defined(CM_SYS_OSX)
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		t.tv_sec = mts.tv_sec;
		t.tv_nsec = mts.tv_nsec;
#else
		// IMPORTANT TODO(bill): THIS IS A HACK
		clock_gettime(0 /*CLOCK_REALTIME*/, &t);
#endif
		return cast(u64)t.tv_sec * 1000000ull + t.tv_nsec/1000 + 11644473600000000ull;
	}

	cm_inline void 
	cm_sleep_ms(u32 ms) {
		struct timespec req = {cast(time_t)ms/1000, cast(long)((ms%1000)*1000000)};
		struct timespec rem = {0, 0};
		nanosleep(&req, &rem);
	}

#endif