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

#ifndef CM_UTF8_H
#define CM_UTF8_H

#include "dll.h"
#include "types.h"

/////////////////////////////////////////////////////////////////////////////////
//
// UTF-8 Handling
//
////////////////////////////////////////////////////////////////////////////////

// NOTE(bill): Does not check if utf-8 string is valid
CM_DEF isize cm_utf8_strlen (u8 const *str);
CM_DEF isize cm_utf8_strnlen(u8 const *str, isize max_len);

// NOTE(bill): Windows doesn't handle 8 bit filenames well ('cause Micro$hit)
CM_DEF u16 *cm_utf8_to_ucs2    (u16 *buffer, isize len, u8 const *str);
CM_DEF u8 * cm_ucs2_to_utf8    (u8 *buffer, isize len, u16 const *str);
CM_DEF u16 *cm_utf8_to_ucs2_buf(u8 const *str);   // NOTE(bill): Uses locally persisting buffer
CM_DEF u8 * cm_ucs2_to_utf8_buf(u16 const *str); // NOTE(bill): Uses locally persisting buffer

// NOTE(bill): Returns size of codepoint in bytes
CM_DEF isize cm_utf8_decode        (u8 const *str, isize str_len, Rune *codepoint);
CM_DEF isize cm_utf8_codepoint_size(u8 const *str, isize str_len);
CM_DEF isize cm_utf8_encode_rune   (u8 buf[4], Rune r);

#endif //CM_UTF8_H