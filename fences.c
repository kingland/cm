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

#include "fences.h"
#include "arch.h"
#include "utils.h"
#include "header.h"

CM_BEGIN_EXTERN

cm_inline void 
cm_yield_thread(void) {
#if defined(CM_SYS_WINDOWS)
	_mm_pause();
#elif defined(CM_SYS_OSX)
	__asm__ volatile ("" : : : "memory");
#elif defined(CM_CPU_X86)
	_mm_pause();
#else
#error Unknown architecture
#endif
}

cm_inline void 
cm_mfence(void) {
#if defined(CM_SYS_WINDOWS)
	_ReadWriteBarrier();
#elif defined(CM_SYS_OSX)
	__sync_synchronize();
#elif defined(CM_CPU_X86)
	_mm_mfence();
#else
#error Unknown architecture
#endif
}

cm_inline void 
cm_sfence(void) {
#if defined(CM_SYS_WINDOWS)
	_WriteBarrier();
#elif defined(CM_SYS_OSX)
	__asm__ volatile ("" : : : "memory");
#elif defined(CM_CPU_X86)
	_mm_sfence();
#else
#error Unknown architecture
#endif
}

cm_inline void 
cm_lfence(void) {
#if defined(CM_SYS_WINDOWS)
	_ReadBarrier();
#elif defined(CM_SYS_OSX)
	__asm__ volatile ("" : : : "memory");
#elif defined(CM_CPU_X86)
	_mm_lfence();
#else
#error Unknown architecture
#endif
}

CM_END_EXTERN