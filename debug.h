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

#ifndef CM_DEBUG_H
#define CM_DEBUG_H

#include "dll.h"
#include "types.h"

#ifndef CM_DEBUG_TRAP
	#if defined(_MSC_VER)
	 	#if _MSC_VER < 1300
		#define CM_DEBUG_TRAP() __asm int 3 /* Trap to debugger! */
		#else
		#define CM_DEBUG_TRAP() __debugbreak()
		#endif
	#else
		#define CM_DEBUG_TRAP() __builtin_trap()
	#endif
#endif

#ifndef CM_ASSERT_MSG
#define CM_ASSERT_MSG(cond, msg, ...) do { \
	if (!(cond)) { \
		cm_assert_handler("Assertion Failure", #cond, __FILE__, cast(i64)__LINE__, msg, ##__VA_ARGS__); \
		CM_DEBUG_TRAP(); \
	} \
} while (0)
#endif

#ifndef CM_ASSERT
#define CM_ASSERT(cond) CM_ASSERT_MSG(cond, NULL)
#endif

#ifndef CM_ASSERT_NOT_NULL
#define CM_ASSERT_NOT_NULL(ptr) CM_ASSERT_MSG((ptr) != NULL, #ptr " must not be NULL")
#endif

// NOTE(bill): Things that shouldn't happen with a message!
#ifndef CM_PANIC
#define CM_PANIC(msg, ...) do { \
	cm_assert_handler("Panic", NULL, __FILE__, cast(i64)__LINE__, msg, ##__VA_ARGS__); \
	CM_DEBUG_TRAP(); \
} while (0)
#endif

CM_DEF void cm_assert_handler(char const *prefix, char const *condition, char const *file, i32 line, char const *msg, ...);

#endif //CM_DEBUG_H