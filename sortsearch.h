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

#ifndef CM_SORT_SEARCH_H
#define CM_SORT_SEARCH_H

#include "dll.h"
#include "types.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////////
//
// Sort & Search
//
/////////////////////////////////////////////////////////////////////////////////

#define CM_COMPARE_PROC(name) int name(void const *a, void const *b)
typedef CM_COMPARE_PROC(cmCompareProc);

#define CM_COMPARE_PROC_PTR(def) CM_COMPARE_PROC((*def))

// Producure pointers
// NOTE(bill): The offset parameter specifies the offset in the structure
// e.g. cm_i32_cmp(cm_offset_of(Thing, value))
// Use 0 if it's just the type instead.

CM_DEF CM_COMPARE_PROC_PTR(cm_i16_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_i32_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_i64_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_isize_cmp(isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_str_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_f32_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_f64_cmp  (isize offset));
CM_DEF CM_COMPARE_PROC_PTR(cm_char_cmp (isize offset));

// TODO(bill): Better sorting algorithms
// NOTE(bill): Uses quick sort for large arrays but insertion sort for small
#define cm_sort_array(array, count, compare_proc) cm_sort(array, count, cm_size_of(*(array)), compare_proc)
CM_DEF void cm_sort(void *base, isize count, isize size, cmCompareProc compare_proc);

// NOTE(bill): the count of temp == count of items
#define cm_radix_sort(Type) cm_radix_sort_##Type
#define CM_RADIX_SORT_PROC(Type) void cm_radix_sort(Type)(Type *items, Type *temp, isize count)

CM_DEF CM_RADIX_SORT_PROC(u8);
CM_DEF CM_RADIX_SORT_PROC(u16);
CM_DEF CM_RADIX_SORT_PROC(u32);
CM_DEF CM_RADIX_SORT_PROC(u64);


// NOTE(bill): Returns index or -1 if not found
#define cm_binary_search_array(array, count, key, compare_proc) cm_binary_search(array, count, cm_size_of(*(array)), key, compare_proc)
CM_DEF isize cm_binary_search(void const *base, isize count, isize size, void const *key, cmCompareProc compare_proc);

#define cm_shuffle_array(array, count) cm_shuffle(array, count, cm_size_of(*(array)))
CM_DEF void cm_shuffle(void *base, isize count, isize size);

#define cm_reverse_array(array, count) cm_reverse(array, count, cm_size_of(*(array)))
CM_DEF void cm_reverse(void *base, isize count, isize size);

#endif //CM_SORT_SEARCH_H