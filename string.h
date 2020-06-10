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

#ifndef CM_STRING_H
#define CM_STRING_H

#include "dll.h"
#include "types.h"
#include "memory.h"

/////////////////////////////////////////////////////////////////////////////////
//
// cmString - C Read-Only-Compatible
//
//////////////////////////////////////////////////////////////////////////////////
/*
Reasoning:

	By default, strings in C are null terminated which means you have to count
	the number of character up to the null character to calculate the length.
	Many "better" C string libraries will create a struct for a string.
	i.e.

	    struct String {
	    	Allocator allocator;
	        size_t    length;
	        size_t    capacity;
	        char *    cstring;
	    };

	This library tries to augment normal C strings in a better way that is still
	compatible with C-style strings.

	+--------+-----------------------+-----------------+
	| Header | Binary C-style String | Null Terminator |
	+--------+-----------------------+-----------------+
	         |
	         +-> Pointer returned by functions

	Due to the meta-data being stored before the string pointer and every cm string
	having an implicit null terminator, cm strings are full compatible with c-style
	strings and read-only functions.

Advantages:

    * cm strings can be passed to C-style string functions without accessing a struct
      member of calling a function, i.e.

          cm_printf("%s\n", cm_str);

      Many other libraries do either of these:

          cm_printf("%s\n", string->cstr);
          cm_printf("%s\n", get_cstring(string));

    * You can access each character just like a C-style string:

          cm_printf("%c %c\n", str[0], str[13]);

    * cm strings are singularly allocated. The meta-data is next to the character
      array which is better for the cache.

Disadvantages:

    * In the C version of these functions, many return the new string. i.e.
          str = cm_string_appendc(str, "another string");
      This could be changed to cm_string_appendc(&str, "another string"); but I'm still not sure.

	* This is incompatible with "cm_string.h" strings
*/

#if 0
#define CM_IMPLEMENTATION
#include "cm.h"
int main(int argc, char **argv) {
	cmString str = cm_string_make("Hello");
	cmString other_str = cm_string_make_length(", ", 2);
	str = cm_string_append(str, other_str);
	str = cm_string_appendc(str, "world!");

	cm_printf("%s\n", str); // Hello, world!

	cm_printf("str length = %d\n", cm_string_length(str));

	str = cm_string_set(str, "Potato soup");
	cm_printf("%s\n", str); // Potato soup

	str = cm_string_set(str, "Hello");
	other_str = cm_string_set(other_str, "Pizza");
	if (cm_strings_are_equal(str, other_str))
		cm_printf("Not called\n");
	else
		cm_printf("Called\n");

	str = cm_string_set(str, "Ab.;!...AHello World       ??");
	str = cm_string_trim(str, "Ab.;!. ?");
	cm_printf("%s\n", str); // "Hello World"

	cm_string_free(str);
	cm_string_free(other_str);

	return 0;
}
#endif

// TODO(bill): Should this be a wrapper to cmArray(char) or this extra type safety better?
typedef char *cmString;

// NOTE(bill): If you only need a small string, just use a standard c string or change the size from isize to u16, etc.
typedef struct cmStringHeader {
	cmAllocator allocator;
	isize       length;
	isize       capacity;
} cmStringHeader;

#define CM_STRING_HEADER(str) (cast(cmStringHeader *)(str) - 1)

CM_DEF cmString cm_string_make_reserve   (cmAllocator a, isize capacity);
CM_DEF cmString cm_string_make           (cmAllocator a, char const *str);
CM_DEF cmString cm_string_make_length    (cmAllocator a, void const *str, isize num_bytes);
CM_DEF void     cm_string_free           (cmString str);
CM_DEF cmString cm_string_duplicate      (cmAllocator a, cmString const str);
CM_DEF isize    cm_string_length         (cmString const str);
CM_DEF isize    cm_string_capacity       (cmString const str);
CM_DEF isize    cm_string_available_space(cmString const str);
CM_DEF void     cm_string_clear          (cmString str);
CM_DEF cmString cm_string_append         (cmString str, cmString const other);
CM_DEF cmString cm_string_append_length  (cmString str, void const *other, isize num_bytes);
CM_DEF cmString cm_string_append_char    (cmString str, char const *other);
CM_DEF cmString cm_string_append_rune    (cmString str, Rune r);
CM_DEF cmString cm_string_append_fmt     (cmString str, char const *fmt, ...);
CM_DEF cmString cm_string_set            (cmString str, char const *cstr);
CM_DEF cmString cm_string_make_space_for (cmString str, isize add_len);
CM_DEF isize    cm_string_allocation_size(cmString const str);
CM_DEF b32      cm_string_are_equal      (cmString const lhs, cmString const rhs);
CM_DEF cmString cm_string_trim           (cmString str, char const *cut_set);
CM_DEF cmString cm_string_trim_space     (cmString str); // Whitespace ` \t\r\n\v\f`

#endif //CM_STRING_H