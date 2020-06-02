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

#ifndef CM_HEADER_H
#define CM_HEADER_H

#include "arch.h"

#if defined(_WIN32) && !defined(__MINGW32__)
	#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif

#if defined(CM_SYS_UNIX)
	#define _GNU_SOURCE
	#define _LARGEFILE64_SOURCE
#endif

// TODO(bill): How many of these headers do I really need?
// #include <stdarg.h>
#if !defined(CM_SYS_WINDOWS)
	#include <stddef.h>
	#include <stdarg.h>
#endif

#if defined(CM_SYS_WINDOWS)
	#if !defined(CM_NO_WINDOWS_H)
		#define NOMINMAX            1
		#if !defined(CM_WINDOWS_H_INCLUDED)
		#define WIN32_LEAN_AND_MEAN 1
		#define WIN32_MEAN_AND_LEAN 1
		#define VC_EXTRALEAN        1
		#endif
		#include <windows.h>
		#undef NOMINMAX
		#if !defined(CM_WINDOWS_H_INCLUDED)
		#undef WIN32_LEAN_AND_MEAN
		#undef WIN32_MEAN_AND_LEAN
		#undef VC_EXTRALEAN
		#endif
	#endif

	#include <malloc.h> // NOTE(bill): _aligned_*()
	#include <intrin.h>
#else
	#include <dlfcn.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <pthread.h>
	#ifndef _IOSC11_SOURCE
	#define _IOSC11_SOURCE
	#endif
	#include <stdlib.h> // NOTE(bill): malloc on linux
	#include <sys/mman.h>
	#if !defined(CM_SYS_OSX)
		#include <sys/sendfile.h>
	#endif
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <time.h>
	#include <unistd.h>

	#if defined(CM_CPU_X86)
		#include <xmmintrin.h>
	#endif
#endif

#if defined(CM_SYS_OSX)
	#include <mach/mach.h>
	#include <mach/mach_init.h>
	#include <mach/mach_time.h>
	#include <mach/thread_act.h>
	#include <mach/thread_policy.h>
	#include <sys/sysctl.h>
	#include <copyfile.h>
	#include <mach/clock.h>
#endif

#if defined(CM_SYS_UNIX)
	#include <semaphore.h>
#endif

#endif //CM_HEADER_H