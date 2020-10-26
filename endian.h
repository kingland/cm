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

#ifndef CM_ENDIAN_H
#define CM_ENDIAN_H

#include "arch.h"
#include "extern.h"

CM_BEGIN_EXTERN

#if CM_SYS_LINUX || CM_SYS_CYGWIN
    #include <endian.h>
#elif CM_SYS_OSX
    #include <libkern/OSByteOrder.h>

    #define htobe16(x) OSSwapHostToBigInt16(x)
    #define htole16(x) OSSwapHostToLittleInt16(x)
    #define be16toh(x) OSSwapBigToHostInt16(x)
    #define le16toh(x) OSSwapLittleToHostInt16(x)

    #define htobe32(x) OSSwapHostToBigInt32(x)
    #define htole32(x) OSSwapHostToLittleInt32(x)
    #define be32toh(x) OSSwapBigToHostInt32(x)
    #define le32toh(x) OSSwapLittleToHostInt32(x)

    #define htobe64(x) OSSwapHostToBigInt64(x)
    #define htole64(x) OSSwapHostToLittleInt64(x)
    #define be64toh(x) OSSwapBigToHostInt64(x)
    #define le64toh(x) OSSwapLittleToHostInt64(x)
#elif CM_SYS_OPENBSD
    #include <sys/endian.h>
#elif CM_SYS_NETBSD || CM_SYS_FREEBSD || CM_SYS_DRAGONFLY
    #include <sys/endian.h>

    #define be16toh(x) betoh16(x)
    #define le16toh(x) letoh16(x)

    #define be32toh(x) betoh32(x)
    #define le32toh(x) letoh32(x)

    #define be64toh(x) betoh64(x)
    #define le64toh(x) letoh64(x)
#endif

#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
    #define CM_BYTE_ORDER    BYTE_ORDER
    #define CM_LITTLE_ENDIAN LITTLE_ENDIAN
    #define CM_BIG_ENDIAN    BIG_ENDIAN
#elif defined(_BYTE_ORDER) && defined(_BIG_ENDIAN) && defined(_LITTLE_ENDIAN)
    #define CM_BYTE_ORDER    _BYTE_ORDER
    #define CM_LITTLE_ENDIAN _LITTLE_ENDIAN
    #define CM_BIG_ENDIAN    _BIG_ENDIAN
#elif defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && defined(__LITTLE_ENDIAN)
    #define CM_BYTE_ORDER    __BYTE_ORDER
    #define CM_LITTLE_ENDIAN __LITTLE_ENDIAN
    #define CM_BIG_ENDIAN    __BIG_ENDIAN
#endif


#ifndef CM_BYTE_ORDER
    #define CM_LITTLE_ENDIAN 1234
    #define CM_BIG_ENDIAN 4321

    //LITTLE_ENDIAN
    #if defined(CM_CPU_X86) || defined(CM_CPU_AI64) || defined(CM_CPU_ALPHA) || defined(CM_CPU_RISCV)
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN
    #elif defined(CM_CPU_PPC) && defined(__LITTLE_ENDIAN__)
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN
    #elif defined(CM_CPU_SH) && defined(__LITTLE_ENDIAN__)
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN
    #elif defined(CM_CPU_MIPS) && defined(__MIPSEL__)
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN    
    #elif defined(CM_CPU_ARC) && defined(__LITTLE_ENDIAN__)
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN
    #elif defined(CM_CPU_ARM) && (defined(__ARMEL__) || defined(__AARCH64EL__))
        #define CM_BYTE_ORDER CM_LITTLE_ENDIAN
    //BIG_ENDIAN
    #elif defined(CM_CPU_SPARC) || defined(CM_CPU_S390) || defined(CM_CPU_HPPA) || defined(CM_CPU_OR1K) || defined(CM_CPU_M68K)
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #elif defined(CM_CPU_PPC) && defined(__BIG_ENDIAN__)
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #elif defined(CM_CPU_ARM) && (defined(__ARMEB__) || defined(__AARCH64EB__))
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #elif defined(CM_CPU_SH) && defined(__BIG_ENDIAN__)
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #elif defined(CM_CPU_MIPS) && defined(__MIPSEB__)
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #elif defined(CM_CPU_ARC) && defined(__BIG_ENDIAN__)
        #define CM_BYTE_ORDER CM_BIG_ENDIAN
    #else
        #error Unknown CPU: can not set endianness
    #endif
#endif

#if CM_SYS_WINDOWS && defined(CM_BYTE_ORDER)
    #include <winsock2.h>

    #if CM_BYTE_ORDER == CM_LITTLE_ENDIAN

    #define htobe16(x) htons(x)
    #define htole16(x) (x)
    #define be16toh(x) ntohs(x)
    #define le16toh(x) (x)

    #define htobe32(x) htonl(x)
    #define htole32(x) (x)
    #define be32toh(x) ntohl(x)
    #define le32toh(x) (x)

    #define htobe64(x) htonll(x)
    #define htole64(x) (x)
    #define be64toh(x) ntohll(x)
    #define le64toh(x) (x)

    #elif CM_BYTE_ORDER == CM_BIG_ENDIAN

    /* that would be xbox 360 */
    #define htobe16(x) (x)
    #define htole16(x) __builtin_bswap16(x)
    #define be16toh(x) (x)
    #define le16toh(x) __builtin_bswap16(x)

    #define htobe32(x) (x)
    #define htole32(x) __builtin_bswap32(x)
    #define be32toh(x) (x)
    #define le32toh(x) __builtin_bswap32(x)

    #define htobe64(x) (x)
    #define htole64(x) __builtin_bswap64(x)
    #define be64toh(x) (x)
    #define le64toh(x) __builtin_bswap64(x)
    #endif
    
#endif

CM_END_EXTERN

#endif

