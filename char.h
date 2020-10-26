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

#ifndef CM_CHAR_H
#define CM_CHAR_H

#include "dll.h"
#include "types.h"

CM_BEGIN_EXTERN

//
// Char Functions
//

CM_DEF char cm_char_to_lower       (char c);
CM_DEF char cm_char_to_upper       (char c);
CM_DEF b32  cm_char_is_space       (char c);
CM_DEF b32  cm_char_is_newline     (char c);
CM_DEF b32  cm_char_is_digit       (char c);
CM_DEF b32  cm_char_is_hex_digit   (char c);
CM_DEF b32  cm_char_is_alpha       (char c);
CM_DEF b32  cm_char_is_alphanumeric(char c);
CM_DEF i32  cm_digit_to_int        (char c);
CM_DEF i32  cm_hex_digit_to_int    (char c);

// NOTE(bill): ASCII only
CM_DEF void cm_str_to_lower (char *str);
CM_DEF void cm_str_to_upper (char *str);

CM_DEF isize cm_strlen (char const *str);
CM_DEF isize cm_strnlen(char const *str, isize max_len);
CM_DEF i32   cm_strcmp (char const *s1, char const *s2);
CM_DEF i32   cm_stricmp(char const *s1, char const *s2);
CM_DEF i32   cm_strncmp(char const *s1, char const *s2, isize len);
CM_DEF i32   cm_strnicmp(char const *s1, char const *s2, isize len);
CM_DEF char *cm_strcpy (char *dest, char const *source);
CM_DEF char *cm_strncpy(char *dest, char const *source, isize len);
CM_DEF isize cm_strlcpy(char *dest, char const *source, isize len);
CM_DEF char *cm_strsub(char const *s, isize n);
CM_DEF char *cm_strrev (char *str); // NOTE(bill): ASCII only

// NOTE(bill): A less fucking crazy strtok!
CM_DEF char const *cm_strtok(char *output, char const *src, char const *delimit);
CM_DEF b32   cm_str_has_prefix  (char const *str, char const *prefix);
CM_DEF isize cm_str_prefix_count(char const *str, char const *prefix);
CM_DEF b32   cm_str_has_suffix  (char const *str, char const *suffix);
CM_DEF b32   cm_str_has_isuffix (char const *str, char const *suffix);

CM_DEF char const *cm_char_first_occurence(char const *str, char c);
CM_DEF char const *cm_char_last_occurence (char const *str, char c);

CM_DEF void cm_str_concat(char *dest, isize dest_len,
                          char const *src_a, isize src_a_len,
                          char const *src_b, isize src_b_len);

CM_DEF u64  cm_str_to_u64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
CM_DEF i64  cm_str_to_i64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
CM_DEF f32  cm_str_to_f32(char const *str, char **end_ptr);
CM_DEF f64  cm_str_to_f64(char const *str, char **end_ptr);
CM_DEF void cm_i64_to_str(i64 value, char *string, i32 base);
CM_DEF void cm_u64_to_str(u64 value, char *string, i32 base);

CM_END_EXTERN

#endif //CM_CHAR_H