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

#include "utf8.h"
#include "utils.h"

CM_BEGIN_EXTERN

cm_inline isize 
cm_utf8_strlen(u8 const *str) {
	isize count = 0;
	for (; *str; count++) {
		u8 c = *str;
		isize inc = 0;
		     if (c < 0x80)           inc = 1;
		else if ((c & 0xe0) == 0xc0) inc = 2;
		else if ((c & 0xf0) == 0xe0) inc = 3;
		else if ((c & 0xf8) == 0xf0) inc = 4;
		else return -1;

		str += inc;
	}
	return count;
}

cm_inline isize 
cm_utf8_strnlen(u8 const *str, isize max_len) {
	isize count = 0;
	for (; *str && max_len > 0; count++) {
		u8 c = *str;
		isize inc = 0;
		     if (c < 0x80)           inc = 1;
		else if ((c & 0xe0) == 0xc0) inc = 2;
		else if ((c & 0xf0) == 0xe0) inc = 3;
		else if ((c & 0xf8) == 0xf0) inc = 4;
		else return -1;

		str += inc;
		max_len -= inc;
	}
	return count;
}

//
// Windows UTF-8 Handling
//

u16 *
cm_utf8_to_ucs2(u16 *buffer, isize len, u8 const *str) {
	Rune c;
	isize i = 0;
	len--;
	while (*str) {
		if (i >= len)
			return NULL;
		if (!(*str & 0x80)) {
			buffer[i++] = *str++;
		} else if ((*str & 0xe0) == 0xc0) {
			if (*str < 0xc2)
				return NULL;
			c = (*str++ & 0x1f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			buffer[i++] = cast(u16)(c + (*str++ & 0x3f));
		} else if ((*str & 0xf0) == 0xe0) {
			if (*str == 0xe0 &&
			    (str[1] < 0xa0 || str[1] > 0xbf))
				return NULL;
			if (*str == 0xed && str[1] > 0x9f) // str[1] < 0x80 is checked below
				return NULL;
			c = (*str++ & 0x0f) << 12;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			buffer[i++] = cast(u16)(c + (*str++ & 0x3f));
		} else if ((*str & 0xf8) == 0xf0) {
			if (*str > 0xf4)
				return NULL;
			if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
				return NULL;
			if (*str == 0xf4 && str[1] > 0x8f) // str[1] < 0x80 is checked below
				return NULL;
			c = (*str++ & 0x07) << 18;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 12;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f) << 6;
			if ((*str & 0xc0) != 0x80)
				return NULL;
			c += (*str++ & 0x3f);
			// UTF-8 encodings of values used in surrogate pairs are invalid
			if ((c & 0xfffff800) == 0xd800)
				return NULL;
			if (c >= 0x10000) {
				c -= 0x10000;
				if (i+2 > len)
					return NULL;
				buffer[i++] = 0xd800 | (0x3ff & (c>>10));
				buffer[i++] = 0xdc00 | (0x3ff & (c    ));
			}
		} else {
			return NULL;
		}
	}
	buffer[i] = 0;
	return buffer;
}

u8 *
cm_ucs2_to_utf8(u8 *buffer, isize len, u16 const *str) {
	isize i = 0;
	len--;
	while (*str) {
		if (*str < 0x80) {
			if (i+1 > len)
				return NULL;
			buffer[i++] = (char) *str++;
		} else if (*str < 0x800) {
			if (i+2 > len)
				return NULL;
			buffer[i++] = cast(char)(0xc0 + (*str >> 6));
			buffer[i++] = cast(char)(0x80 + (*str & 0x3f));
			str += 1;
		} else if (*str >= 0xd800 && *str < 0xdc00) {
			Rune c;
			if (i+4 > len)
				return NULL;
			c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
			buffer[i++] = cast(char)(0xf0 +  (c >> 18));
			buffer[i++] = cast(char)(0x80 + ((c >> 12) & 0x3f));
			buffer[i++] = cast(char)(0x80 + ((c >>  6) & 0x3f));
			buffer[i++] = cast(char)(0x80 + ((c      ) & 0x3f));
			str += 2;
		} else if (*str >= 0xdc00 && *str < 0xe000) {
			return NULL;
		} else {
			if (i+3 > len)
				return NULL;
			buffer[i++] = 0xe0 +  (*str >> 12);
			buffer[i++] = 0x80 + ((*str >>  6) & 0x3f);
			buffer[i++] = 0x80 + ((*str      ) & 0x3f);
			str += 1;
		}
	}
	buffer[i] = 0;
	return buffer;
}

u16 *
cm_utf8_to_ucs2_buf(u8 const *str) { // NOTE(bill): Uses locally persisting buffer
	cm_local_persist u16 buf[4096];
	return cm_utf8_to_ucs2(buf, cm_count_of(buf), str);
}

u8 *
cm_ucs2_to_utf8_buf(u16 const *str) { // NOTE(bill): Uses locally persisting buffer
	cm_local_persist u8 buf[4096];
	return cm_ucs2_to_utf8(buf, cm_count_of(buf), str);
}

cm_global u8 const 
cm__utf8_first[256] = {
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x00-0x0F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x10-0x1F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x20-0x2F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x30-0x3F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x40-0x4F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x50-0x5F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x60-0x6F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // 0x70-0x7F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, // 0x80-0x8F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, // 0x90-0x9F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, // 0xA0-0xAF
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, // 0xB0-0xBF
	0xf1, 0xf1, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // 0xC0-0xCF
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // 0xD0-0xDF
	0x13, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x23, 0x03, 0x03, // 0xE0-0xEF
	0x34, 0x04, 0x04, 0x04, 0x44, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, // 0xF0-0xFF
};

typedef struct cmUtf8AcceptRange {
	u8 lo, hi;
} cmUtf8AcceptRange;

cm_global cmUtf8AcceptRange const 
cm__utf8_accept_ranges[] = {
	{0x80, 0xbf},
	{0xa0, 0xbf},
	{0x80, 0x9f},
	{0x90, 0xbf},
	{0x80, 0x8f},
};

isize 
cm_utf8_decode(u8 const *str, isize str_len, Rune *codepoint_out) {
	isize width = 0;
	Rune codepoint = CM_RUNE_INVALID;

	if (str_len > 0) {
		u8 s0 = str[0];
		u8 x = cm__utf8_first[s0], sz;
		u8 b1, b2, b3;
		cmUtf8AcceptRange accept;
		if (x >= 0xf0) {
			Rune mask = (cast(Rune)x << 31) >> 31;
			codepoint = (cast(Rune)s0 & (~mask)) | (CM_RUNE_INVALID & mask);
			width = 1;
			goto end;
		}
		if (s0 < 0x80) {
			codepoint = s0;
			width = 1;
			goto end;
		}

		sz = x&7;
		accept = cm__utf8_accept_ranges[x>>4];
		if (str_len < cm_size_of(sz))
			goto invalid_codepoint;

		b1 = str[1];
		if (b1 < accept.lo || accept.hi < b1)
			goto invalid_codepoint;

		if (sz == 2) {
			codepoint = (cast(Rune)s0&0x1f)<<6 | (cast(Rune)b1&0x3f);
			width = 2;
			goto end;
		}

		b2 = str[2];
		if (!CM_IS_BETWEEN(b2, 0x80, 0xbf))
			goto invalid_codepoint;

		if (sz == 3) {
			codepoint = (cast(Rune)s0&0x1f)<<12 | (cast(Rune)b1&0x3f)<<6 | (cast(Rune)b2&0x3f);
			width = 3;
			goto end;
		}

		b3 = str[3];
		if (!CM_IS_BETWEEN(b3, 0x80, 0xbf))
			goto invalid_codepoint;

		codepoint = (cast(Rune)s0&0x07)<<18 | (cast(Rune)b1&0x3f)<<12 | (cast(Rune)b2&0x3f)<<6 | (cast(Rune)b3&0x3f);
		width = 4;
		goto end;

	invalid_codepoint:
		codepoint = CM_RUNE_INVALID;
		width = 1;
	}

end:
	if (codepoint_out) *codepoint_out = codepoint;
	return width;
}

isize 
cm_utf8_codepoint_size(u8 const *str, isize str_len) {
	isize i = 0;
	for (; i < str_len && str[i]; i++) {
		if ((str[i] & 0xc0) != 0x80)
			break;
	}
	return i+1;
}

isize 
cm_utf8_encode_rune(u8 buf[4], Rune r) {
	u32 i = cast(u32)r;
	u8 mask = 0x3f;
	if (i <= (1<<7)-1) {
		buf[0] = cast(u8)r;
		return 1;
	}
	if (i <= (1<<11)-1) {
		buf[0] = 0xc0 | cast(u8)(r>>6);
		buf[1] = 0x80 | (cast(u8)(r)&mask);
		return 2;
	}

	// Invalid or Surrogate range
	if (i > CM_RUNE_MAX ||
	    CM_IS_BETWEEN(i, 0xd800, 0xdfff)) {
		r = CM_RUNE_INVALID;

		buf[0] = 0xe0 | cast(u8)(r>>12);
		buf[1] = 0x80 | (cast(u8)(r>>6)&mask);
		buf[2] = 0x80 | (cast(u8)(r)&mask);
		return 3;
	}

	if (i <= (1<<16)-1) {
		buf[0] = 0xe0 | cast(u8)(r>>12);
		buf[1] = 0x80 | (cast(u8)(r>>6)&mask);
		buf[2] = 0x80 | (cast(u8)(r)&mask);
		return 3;
	}

	buf[0] = 0xf0 | cast(u8)(r>>18);
	buf[1] = 0x80 | (cast(u8)(r>>12)&mask);
	buf[2] = 0x80 | (cast(u8)(r>>6)&mask);
	buf[3] = 0x80 | (cast(u8)(r)&mask);
	return 4;
}

CM_END_EXTERN