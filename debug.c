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

#include "debug.h"
#include "header.h"
#include "print.h"

CM_BEGIN_EXTERN

void 
cm_assert_handler(char const *prefix, char const *condition, char const *file, i32 line, char const *msg, ...) {
	cm_printf_err("%s(%d): %s: ", file, line, prefix);
	if (condition)
		cm_printf_err( "`%s` ", condition);
	if (msg) {
		va_list va;
		va_start(va, msg);
		cm_printf_err_va(msg, va);
		va_end(va);
	}
	cm_printf_err("\n");
}

CM_END_EXTERN