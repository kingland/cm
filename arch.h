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

#if defined(_WIN64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__) || defined(__M_AMD64) || defined(_M_ARM64) || defined(__aarch64__) || (defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64)
	#ifndef CM_ARCH_64
	#define CM_ARCH_64 1
	#endif
#else
	// NOTE(bill): I'm only supporting 32 bit and 64 bit systems
	#ifndef CM_ARCH_32
	#define CM_ARCH_32 1
	#endif
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
	#elif defined(__NetBSD__)
		#ifndef CM_NETBSD
		#define CM_NETBSD 1
		#endif
		#ifndef CM_OS_NETBSD
		#define CM_OS_NETBSD 1
		#endif
		#ifndef CM_SYS_NETBSD
		#define CM_SYS_NETBSD 1
		#endif
	#elif defined(__OpenBSD__)
		#ifndef CM_OPENBSD
		#define CM_OPENBSD 1
		#endif
		#ifndef CM_OS_OPENBSD
		#define CM_OS_OPENBSD 1
		#endif
		#ifndef CM_SYS_OPENBSD
		#define CM_SYS_OPENBSD 1
		#endif
	#elif defined(__DragonFly__)
		#ifndef CM_DRAGONFLY
		#define CM_DRAGONFLY 1
		#endif
		#ifndef CM_OS_DRAGONFLY
		#define CM_OS_DRAGONFLY 1
		#endif
		#ifndef CM_SYS_DRAGONFLY
		#define CM_SYS_DRAGONFLY 1
		#endif	
	#else
		#error This UNIX operating system is not supported
	#endif
#elif defined(__ANDROID__)
	#ifndef CM_ANDROID
	#define CM_ANDROID 1
	#endif
	#ifndef CM_OS_ANDROID
	#define CM_OS_ANDROID 1
	#endif
	#ifndef CM_SYS_ANDROID
	#define CM_SYS_ANDROID 1
	#endif
#elif defined(__CYGWIN__)
	#ifndef CM_CYGWIN
	#define CM_CYGWIN 1
	#endif
	#ifndef CM_SYS_CYGWIN
	#define CM_SYS_CYGWIN 1
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

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__) || defined(__i486__)
	#ifndef CM_CPU_X86
	#define CM_CPU_X86 1
	#define CM_X86 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#elif defined(_M_PPC) || defined(__powerpc__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(__powerpc64__) 
	#ifndef CM_CPU_PPC
	#define CM_CPU_PPC 1
	#define CM_PPC 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 128
	#endif

#elif defined(__arm__) || defined(_M_ARM) || defined(__aarch64__)  || defined(_M_ARM64)
	#ifndef CM_CPU_ARM
	#define CM_CPU_ARM 1
	#define CM_ARM 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#elif defined(__MIPSEL__) || defined(__MIPSEB__) || defined(__mips_isa_rev)
	#ifndef CM_CPU_MIPS
	#define CM_CPU_MIPS 1
	#define CM_MIPS 1
	#endif
	#ifndef CM_CACHE_LINE_SIZE
	#define CM_CACHE_LINE_SIZE 64
	#endif

#elif defined(__sparc__) || defined(__sparc)
	#ifndef CM_CPU_SPARC
	#define CM_CPU_SPARC 1
	#define CM_SPARC 1
	#endif

#elif defined(__s390__)
	#ifndef CM_CPU_S390
    #define CM_CPU_S390 1
	#define CM_S390 1
	#endif

#elif defined(__ia64)
	#ifndef CM_CPU_IA64
	#define CM_CPU_IA64 1
	#define CM_IA64 1
	#endif

#elif defined(__hppa)
	#ifndef CM_CPU_HPPA
	#define CM_CPU_HPPA 1
	#define CM_HPPA 1
	#endif

#elif defined(__alpha__)
	#ifndef CM_CPU_ALPHA
	#define CM_CPU_ALPHA 1
	#define CM_ALPHA 1
	#endif

#elif defined(__sh__)
	#ifndef CM_CPU_SH
	#define CM_CPU_SH 1
	#define CM_SH 1
	#endif

#elif defined(__MIPSEL__)
	#ifndef CM_CPU_MIPSEL
    #define CM_CPU_MIPSEL 1
	#define CM_MIPSEL 1
	#endif

#elif defined(__MIPSEB__)
	#ifndef CM_CPU_MIPSEB
    #define CM_CPU_MIPSEB 1
	#define CM_MIPSEB 1
	#endif

#elif defined(__or1k__)
	#ifndef CM_CPU_OR1K 
    #define CM_CPU_OR1K 1
	#define CM_OR1K 1
	#endif

#elif defined(__mc68000__)
	#ifndef CM_CPU_M68K
    #define CM_CPU_M68K 1
	#define CM_M68K 1
	#endif

#elif defined(__arc__)
	#ifndef CM_CPU_ARC
	#define CM_CPU_ARC 1
	#define CM_ARC 1
	#endif

#elif defined(__riscv)
	#ifndef CM_CPU_RISCV
	#define CM_CPU_RISCV 1
	#define CM_RISCV 1
	#endif
#else
	#error Unknown CPU Type
#endif

#endif //CM_ARCH_H
