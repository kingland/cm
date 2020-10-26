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

#include "dll.h"
#include "arch.h"
#include "header.h"
#include "utils.h"


#if defined(CM_SYS_WINDOWS)

cmDllHandle 
cm_dll_load(char const *filepath) {
	return cast(cmDllHandle) LoadLibraryA(filepath);
}

cm_inline void      
cm_dll_unload(cmDllHandle dll){ 
	FreeLibrary(cast(HMODULE)dll); 
}

cm_inline cmDllProc 
cm_dll_proc_address(cmDllHandle dll, char const *proc_name) { 
	return cast(cmDllProc) GetProcAddress(cast(HMODULE)dll, proc_name); 
}

#else // POSIX

cmDllHandle 
cm_dll_load(char const *filepath) {
	// TODO(bill): Should this be RTLD_LOCAL?
	return cast(cmDllHandle)dlopen(filepath, RTLD_LAZY|RTLD_GLOBAL);
}

cm_inline void      
cm_dll_unload (cmDllHandle dll){ 
	dlclose(dll); 
}

cm_inline cmDllProc 
cm_dll_proc_address(cmDllHandle dll, char const *proc_name) { 
	return cast(cmDllProc)dlsym(dll, proc_name); 
}

#endif