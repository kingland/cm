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

#ifndef CM_PRINT_H
#define CM_PRINT_H

#include "dll.h"
#include "file.h"
#include "header.h"

//////////////////////////////////////////////////////////////////////////////////
//
// Printing
//
//////////////////////////////////////////////////////////////////////////////////

CM_DEF isize cm_printf        (char const *fmt, ...) CM_PRINTF_ARGS(1);
CM_DEF isize cm_printf_va     (char const *fmt, va_list va);
CM_DEF isize cm_printf_err    (char const *fmt, ...) CM_PRINTF_ARGS(1);
CM_DEF isize cm_printf_err_va (char const *fmt, va_list va);
CM_DEF isize cm_fprintf       (cmFile *f, char const *fmt, ...) CM_PRINTF_ARGS(2);
CM_DEF isize cm_fprintf_va    (cmFile *f, char const *fmt, va_list va);

CM_DEF char *cm_bprintf    (char const *fmt, ...) CM_PRINTF_ARGS(1); // NOTE(bill): A locally persisting buffer is used internally
CM_DEF char *cm_bprintf_va (char const *fmt, va_list va);            // NOTE(bill): A locally persisting buffer is used internally
CM_DEF isize cm_snprintf   (char *str, isize n, char const *fmt, ...) CM_PRINTF_ARGS(3);
CM_DEF isize cm_snprintf_va(char *str, isize n, char const *fmt, va_list va);

#endif //CM_PRINT_H