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
// NOTE
//
/////////////////////////////////////////////////////////////////////////////////
//
// https://www.twilio.com/docs/glossary/what-is-ucs-2-character-encoding
//
// UCS-2 is a character encoding standard in which characters are represented by a fixed-length 16 bits (2 bytes).  
// It is used as a fallback on many GSM networks when a message cannot be encoded using GSM-7 
// or when a language requires more than 128 characters to be rendered.
//
// The Basics of UCS-2 Encoding and SMS Messages
// UCS-2 and the other UCS standards are defined by the International Organization for Standardization (ISO) in ISO 10646. 
// UCS-2 represents a possible maximum of 65,536 characters, or in hexadecimals from 0000h - FFFFh (2 bytes). 
// The characters in UCS-2 are synchronized to the Basic Multilingual Plane in Unicode.
//
// UCS-2 is a fixed-width encoding; each encoded code point will take exactly 2 bytes. 
// As a SMS message is transmitted in 140 octets, a message which is encoded in UCS-2 has a maximum of 70 characters 
// (really, code points): (140*8) / (2*8) = 70.
//
/////////////////////////////////////////////////////////////////////////////////
//
// http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
// http://www.unicode.org/notes/tn12/#U4ch2
//
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// UTF-8 Handling
//
////////////////////////////////////////////////////////////////////////////////

CM_BEGIN_EXTERN

CM_DEF isize    cm_utf8_strlen      (u8 const *str); // NOTE(bill): Does not check if utf-8 string is valid
CM_DEF isize    cm_utf8_strnlen     (u8 const *str, isize max_len);

CM_DEF u16*     cm_utf8_to_ucs2     (u16 *buffer, isize len, u8 const *str); // NOTE(bill): Windows doesn't handle 8 bit filenames well ('cause Micro$hit)
CM_DEF u8*      cm_ucs2_to_utf8     (u8 *buffer, isize len, u16 const *str);
CM_DEF u16*     cm_utf8_to_ucs2_buf (u8 const *str);   // NOTE(bill): Uses locally persisting buffer
CM_DEF u8*      cm_ucs2_to_utf8_buf (u16 const *str); // NOTE(bill): Uses locally persisting buffer

CM_DEF bool     cm_utf16_is_bmp     (u16 const codepoint);  // NOTE(): UTF16 code point
CM_DEF bool     cm_utf16_is_high_surrogate(u16 const codepoint); //NOTE(): UTF16 code point
CM_DEF bool     cm_utf16_is_low_surrogate(u16 const codepoint); //NOTE(): UTF16 code point
CM_DEF b8       cm_utf16_surrogate_pair_decode(u16 const high, u16 const low, Rune *codepoint); //NOTE: return negative is error

CM_DEF isize    cm_utf8_decode      (u8 const *str, isize str_len, Rune *codepoint); // NOTE(bill): Returns size of codepoint in bytes
CM_DEF isize    cm_utf8_dfa_decode  (u8 const *str, isize str_len, Rune *codepoint); // NOTE:
CM_DEF isize    cm_utf8_codepoint_size(u8 const *str, isize str_len);
CM_DEF isize    cm_utf8_encode_rune (u8 buf[4], Rune r); //NOTE: rune to utf8

CM_DEF bool     cm_rune_is_valid    (Rune const r);
CM_DEF bool     cm_rune_is_space    (Rune const r);
CM_DEF bool     cm_rune_is_digit    (Rune const r);
CM_DEF bool     cm_rune_is_alpha    (Rune const r);
CM_DEF bool     cm_rune_is_upper    (Rune const r);
CM_DEF bool     cm_rune_is_lowwer   (Rune const r);
CM_DEF bool     cm_rune_is_title    (Rune const r);
CM_DEF bool     cm_rune_is_line_terminate(Rune const r);
CM_DEF Rune     cm_rune_to_upper    (Rune const r);
CM_DEF Rune     cm_rune_to_lowwer   (Rune const r);
CM_DEF Rune     cm_rune_to_title    (Rune const r);

CM_END_EXTERN

#endif //CM_UTF8_H