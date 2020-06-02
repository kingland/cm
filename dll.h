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

#ifndef CM_DLL_H
#define CM_DLL_H

#include "extern.h"

#if defined(_WIN32)
	#define CM_DLL_EXPORT CM_EXTERN __declspec(dllexport)
	#define CM_DLL_IMPORT CM_EXTERN __declspec(dllimport)
#else
	#define CM_DLL_EXPORT CM_EXTERN __attribute__((visibility("default")))
	#define CM_DLL_IMPORT CM_EXTERN
#endif

#ifndef CM_SHARED_LIB_BUILD
	#define CM_SHARED_LIB_BUILD 0
#endif

#ifndef CM_SHARED_LIB_USE
	#define CM_SHARED_LIB_USE 0
#endif

#if CM_SHARED_LIB_BUILD
	#define CM_C_API CM_DLL_EXPORT
#elif CM_SHARED_LIB_USE
	#define CM_C_API CM_DLL_IMPORT
#else
	#define CM_C_API
#endif

// NOTE(bill): Redefine for DLL, etc.
#ifndef CM_DEF
//	#ifdef CM_STATIC
//		#define CM_DEF static
//	#else
		//#define CM_DEF extern
//		#define CM_DEF CM_C_API
//	#endif
	#define CM_DEF CM_C_API
#endif

//
// DLL Handling
//

typedef void *cmDllHandle;
typedef void (*cmDllProc)(void);

CM_DEF cmDllHandle cm_dll_load        (char const *filepath);
CM_DEF void        cm_dll_unload      (cmDllHandle dll);
CM_DEF cmDllProc   cm_dll_proc_address(cmDllHandle dll, char const *proc_name);

#endif //CM_DLL_H