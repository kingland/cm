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

#include "misc.h"
#include "utils.h"
#include "header.h"

CM_BEGIN_EXTERN

#if defined(CM_SYS_WINDOWS)
cm_inline void cm_exit(u32 code) { ExitProcess(code); }
#else
cm_inline void cm_exit(u32 code) { exit(code); }
#endif

cm_inline void cm_yield(void) {
#if defined(CM_SYS_WINDOWS)
	YieldProcessor();
#else
	sched_yield();
#endif
}

cm_inline void 
cm_set_env(char const *name, char const *value) {
#if defined(CM_SYS_WINDOWS)
	// TODO(bill): Should this be a Wide version?
	SetEnvironmentVariableA(name, value);
#else
	setenv(name, value, 1);
#endif
}

cm_inline void 
cm_unset_env(char const *name) {
#if defined(CM_SYS_WINDOWS)
	// TODO(bill): Should this be a Wide version?
	SetEnvironmentVariableA(name, NULL);
#else
	unsetenv(name);
#endif
}

cm_inline u16 
cm_endian_swap16(u16 i) {
	return (i>>8) | (i<<8);
}

cm_inline u32 
cm_endian_swap32(u32 i) {
	return (i>>24) |(i<<24) |
	       ((i&0x00ff0000u)>>8)  | ((i&0x0000ff00u)<<8);
}

cm_inline u64 
cm_endian_swap64(u64 i) {
	return (i>>56) | (i<<56) |
	       ((i&0x00ff000000000000ull)>>40) | ((i&0x000000000000ff00ull)<<40) |
	       ((i&0x0000ff0000000000ull)>>24) | ((i&0x0000000000ff0000ull)<<24) |
	       ((i&0x000000ff00000000ull)>>8)  | ((i&0x00000000ff000000ull)<<8);
}

cm_inline isize 
cm_count_set_bits(u64 mask) {
	isize count = 0;
	while (mask) {
		count += (mask & 1);
		mask >>= 1;
	}
	return count;
}

CM_END_EXTERN