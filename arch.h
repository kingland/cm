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

#ifndef CM_ARCH_H
#define CM_ARCH_H

#if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
	#ifndef CM_ARCH_64
	#define CM_ARCH_64 1
	#endif
#else
	// NOTE(bill): I'm only supporting 32 bit and 64 bit systems
	#ifndef CM_ARCH_32
	#define CM_ARCH_32 1
	#endif
#endif

#ifndef CM_ENDIAN_ORDER
#define CM_ENDIAN_ORDER
	// TODO(bill): Is the a good way or is it better to test for certain compilers and macros?
	#define CM_BIG_ENDIAN    (!*(u8*)&(u16){1})
	#define CM_LITTLE_ENDIAN (!CM_BIG_ENDIAN)
#endif

#if defined(_WIN32) || defined(_WIN64)
	#ifndef CM_WINDOWS
	#define CM_WINDOWS 1
	#endif
	#ifndef CM_OS_WINDOWS
	#define CM_OS_WINDOWS 1
	#endif
	#ifndef CM_SYS_WINDOWS
	#define CM_SYS_WINDOWS 1
	#endif
#elif defined(__APPLE__) && defined(__MACH__)
	#ifndef CM_OSX
	#define CM_OSX 1
	#endif
	#ifndef CM_OS_OSX
	#define CM_OS_OSX 1
	#endif
	#ifndef CM_SYS_OSX
	#define CM_SYS_OSX 1
	#endif
#elif defined(__unix__)
	#ifndef CM_UNIX
	#define CM_UNIX 1
	#endif
	#ifndef CM_OS_UNIX
	#define CM_OS_UNIX 1
	#endif
	#ifndef CM_SYS_UNIX
	#define CM_SYS_UNIX 1
	#endif
	#if defined(__linux__)
		#ifndef CM_LINUX
		#define CM_LINUX 1
		#endif
		#ifndef CM_OS_LINUX
		#define CM_OS_LINUX 1
		#endif
		#ifndef CM_SYS_LINUX
		#define CM_SYS_LINUX 1
		#endif
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
		#ifndef CM_FREEBSD
		#define CM_FREEBSD 1
		#endif
		#ifndef CM_OS_FREEBSD
		#define CM_OS_FREEBSD 1		
		#endif
		#ifndef CM_SYS_FREEBSD
		#define CM_SYS_FREEBSD 1
		#endif
	#else
		#error This UNIX operating system is not supported
	#endif
#else
	#error This operating system is not supported
#endif

#if defined(_MSC_VER)
	#define CM_COMPILER_MSVC 1
	#define CM_MSVC 1
#elif defined(__GNUC__)
	#define CM_COMPILER_GCC 1
	#define CM_GCC 1
#elif defined(__clang__)
	#define CM_COMPILER_CLANG 1
	#define CM_CLANG 1
#else
	#error Unknown compiler
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
	#ifndef CM_CPU_X86
	#define CM_CPU_X86 1
	#define CM_X86 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
	#ifndef CM_CPU_PPC
	#define CM_CPU_PPC 1
	#define CM_PPC 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 128
	#endif

#elif defined(__arm__)
	#ifndef CM_CPU_ARM
	#define CM_CPU_ARM 1
	#define CM_ARM 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#elif defined(__MIPSEL__) || defined(__mips_isa_rev)
	#ifndef CM_CPU_MIPS
	#define CM_CPU_MIPS 1
	#define CM_MIPS 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#else
	#error Unknown CPU Type
#endif

#endif //CM_ARCH_H