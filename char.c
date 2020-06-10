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

#include "char.h"
#include "utils.h"
#include "memory.h"
#include "header.h"
#include "debug.h"

CM_BEGIN_EXTERN

#define CM__ONES        (cast(usize)-1/U8_MAX)
#define CM__HIGHS       (CM__ONES * (U8_MAX/2+1))
#define CM__HAS_ZERO(x) ((x)-CM__ONES & ~(x) & CM__HIGHS)

#if defined(CM_COMPILER_MSVC) && defined(_MSC_VER) && (_MSC_VER >= 1300)
#define cm__p_stricmp    _stricmp
#define cm__p_strnicmp   _strnicmp
#define cm__p_strdup     _strdup
#else
#define cm__p_strdup     strdup
#define cm__p_stricmp    stricmp
#define cm__p_strnicmp   strnicmp
#endif

#ifdef CM_SYS_WINDOWS
   #define cm__stricmp(a,b) cm__p_stricmp(a,b)
   #define cm__strnicmp(a,b,n) cm__p_strnicmp(a,b,n)
#else
   #define cm__stricmp(a,b) strcasecmp(a,b) //strcasecmp, strncasecmp - case-insensitive string comparisons
   #define cm__strnicmp(a,b,n) strncasecmp(a,b,n)
#endif

//
// Char things
//

cm_inline char 
cm_char_to_lower(char c) {
	if (c >= 'A' && c <= 'Z')
		return 'a' + (c - 'A');
	return c;
}

cm_inline char 
cm_char_to_upper(char c) {
	if (c >= 'a' && c <= 'z')
		return 'A' + (c - 'a');
	return c;
}

cm_inline b32 
cm_char_is_space(char c) {
	if (c == ' '  ||
	    c == '\t' ||
	    c == '\n' ||
	    c == '\r' ||
	    c == '\f' ||
	    c == '\v')
	    return true;
	return false;
}

cm_inline b32
cm_char_is_newline(char c) {
	if (c == '\n' || c == '\r')
		return true;
	return false;
}

cm_inline b32 
cm_char_is_digit(char c) {
	if (c >= '0' && c <= '9')
		return true;
	return false;
}

cm_inline b32 
cm_char_is_hex_digit(char c) {
	if (cm_char_is_digit(c) ||
	    (c >= 'a' && c <= 'f') ||
	    (c >= 'A' && c <= 'F'))
	    return true;
	return false;
}

cm_inline b32 
cm_char_is_alpha(char c) {
	if ((c >= 'A' && c <= 'Z') ||
	    (c >= 'a' && c <= 'z'))
	    return true;
	return false;
}

cm_inline b32 
cm_char_is_alphanumeric(char c) {
	return cm_char_is_alpha(c) || cm_char_is_digit(c);
}

cm_inline i32 
cm_digit_to_int(char c) {
	return cm_char_is_digit(c) ? c - '0' : c - 'W';
}

cm_inline i32 
cm_hex_digit_to_int(char c) {
	if (cm_char_is_digit(c))
		return cm_digit_to_int(c);
	else if (CM_IS_BETWEEN(c, 'a', 'f'))
		return c - 'a' + 10;
	else if (CM_IS_BETWEEN(c, 'A', 'F'))
		return c - 'A' + 10;
	return -1;
}

cm_inline void 
cm_str_to_lower(char *str) {
	if (!str) return;
	while (*str) {
		*str = cm_char_to_lower(*str);
		str++;
	}
}

cm_inline void 
cm_str_to_upper(char *str) {
	if (!str) return;
	while (*str) {
		*str = cm_char_to_upper(*str);
		str++;
	}
}

cm_inline isize 
cm_strlen(char const *str) {
	char const *begin = str;
	isize const *w;
	if (str == NULL)  {
		return 0;
	}
	while (cast(uintptr)str % sizeof(usize)) {
		if (!*str)
			return str - begin;
		str++;
	}
	w = cast(isize const *)str;
	while (!CM__HAS_ZERO(*w)) {
		w++;
	}
	str = cast(char const *)w;
	while (*str) {
		str++;
	}
	return str - begin;
}

cm_inline isize 
cm_strnlen(char const *str, isize max_len) {
	char const *end = cast(char const *)cm_memchr(str, 0, max_len);
	if (end) {
		return end - str;
	}
	return max_len;
}

cm_inline i32 
cm_strcmp(char const *s1, char const *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++, s2++;
	}
	return *(u8 *)s1 - *(u8 *)s2;
}

cm_inline i32
cm_stricmp(char const *s1, char const *s2){
	return cm__stricmp(s1, s2);
}

cm_inline char *
cm_strcpy(char *dest, char const *source) {
	CM_ASSERT_NOT_NULL(dest);
	if (source) {
		char *str = dest;
		while (*source) *str++ = *source++;
	}
	return dest;
}

cm_inline char *
cm_strncpy(char *dest, char const *source, isize len) {
	CM_ASSERT_NOT_NULL(dest);
	if (source) {
		char *str = dest;
		while (len > 0 && *source) {
			*str++ = *source++;
			len--;
		}
		while (len > 0) {
			*str++ = '\0';
			len--;
		}
	}
	return dest;
}

cm_inline isize 
cm_strlcpy(char *dest, char const *source, isize len) {
	isize result = 0;
	CM_ASSERT_NOT_NULL(dest);
	if (source) {
		char const *source_start = source;
		char *str = dest;
		while (len > 0 && *source) {
			*str++ = *source++;
			len--;
		}
		while (len > 0) {
			*str++ = '\0';
			len--;
		}

		result = source - source_start;
	}
	return result;
}

cm_inline char *
cm_strrev(char *str) {
	isize len = cm_strlen(str);
	char *a = str + 0;
	char *b = str + len-1;
	len /= 2;
	while (len--) {
		cm_swap(char, *a, *b);
		a++, b--;
	}
	return str;
}

cm_inline char *
cm_strsub(char const *s, isize n){
	char *a;
	isize z = cm_strlen(s);
	if (z < n) n = z;
	a = (char *) cm_malloc(n+1);	
	cm_strncpy(a, s, n);
	a[n] = 0;
	return a;
}

cm_inline i32 
cm_strncmp(char const *s1, char const *s2, isize len) {
	for (; len > 0;  s1++, s2++, len--) {
		if (*s1 != *s2) {
			return ((s1 < s2) ? -1 : +1);
		} else if (*s1 == '\0') {
			return 0;
		}
	}
	return 0;
}

cm_inline i32 
cm_strnicmp(char const *s1, char const *s2, isize len) {
	return cm__strnicmp(s1, s2, len);
}

cm_inline char const *
cm_strtok(char *output, char const *src, char const *delimit) {
	while (*src && cm_char_first_occurence(delimit, *src) != NULL) {
		*output++ = *src++;
	}

	*output = 0;
	return *src ? src+1 : src;
}

cm_inline b32 
cm_str_has_prefix(char const *str, char const *prefix) {
	while (*prefix) {
		if (*str++ != *prefix++) {
			return false;
		}
	}
	return true;
}

cm_inline isize 
cm_str_prefix_count(char const *str, char const *prefix){
	isize c=0;
   while (*prefix) {
      if (*str++ != *prefix++)
         break;
      ++c;
   }
   return c;
}

cm_inline b32 
cm_str_has_suffix(char const *str, char const *suffix) {
	isize i = cm_strlen(str);
	isize j = cm_strlen(suffix);
	if (j <= i) {
		return cm_strcmp(str+i-j, suffix) == 0;
	}
	return false;
}

cm_inline b32 
cm_str_has_isuffix(char const *str, char const *suffix) {
	isize i = cm_strlen(str);
	isize j = cm_strlen(suffix);
	if (j <= i) {
		return cm_stricmp(str+i-j, suffix) == 0;
	}
	return false;
}

cm_inline char const *
cm_char_first_occurence(char const *s, char c) {
	char ch = c;
	for (; *s != ch; s++) {
		if (*s == '\0') {
			return NULL;
		}
	}
	return s;
}

cm_inline char const *
cm_char_last_occurence(char const *s, char c) {
	char const *result = NULL;
	do {
		if (*s == c) {
			result = s;
		}
	} while (*s++);

	return result;
}

cm_inline void 
cm_str_concat(char *dest, isize dest_len,
                             char const *src_a, isize src_a_len,
                             char const *src_b, isize src_b_len) {
	CM_ASSERT(dest_len >= src_a_len+src_b_len+1);
	if (dest) {
		cm_memcopy(dest, src_a, src_a_len);
		cm_memcopy(dest+src_a_len, src_b, src_b_len);
		dest[src_a_len+src_b_len] = '\0';
	}
}

cm_internal isize 
cm__scan_i64(char const *text, i32 base, i64 *value) {
	char const *text_begin = text;
	i64 result = 0;
	b32 negative = false;

	if (*text == '-') {
		negative = true;
		text++;
	}

	if (base == 16 && cm_strncmp(text, "0x", 2) == 0) {
		text += 2;
	}

	for (;;) {
		i64 v;
		if (cm_char_is_digit(*text)) {
			v = *text - '0';
		} else if (base == 16 && cm_char_is_hex_digit(*text)) {
			v = cm_hex_digit_to_int(*text);
		} else {
			break;
		}

		result *= base;
		result += v;
		text++;
	}

	if (value) {
		if (negative) result = -result;
		*value = result;
	}

	return (text - text_begin);
}

cm_internal isize 
cm__scan_u64(char const *text, i32 base, u64 *value) {
	char const *text_begin = text;
	u64 result = 0;

	if (base == 16 && cm_strncmp(text, "0x", 2) == 0) {
		text += 2;
	}

	for (;;) {
		u64 v;
		if (cm_char_is_digit(*text)) {
			v = *text - '0';
		} else if (base == 16 && cm_char_is_hex_digit(*text)) {
			v = cm_hex_digit_to_int(*text);
		} else {
			break;
		}

		result *= base;
		result += v;
		text++;
	}

	if (value) *value = result;
	return (text - text_begin);
}

// TODO(bill): Make better
u64 
cm_str_to_u64(char const *str, char **end_ptr, i32 base) {
	isize len;
	u64 value = 0;

	if (!base) {
		if ((cm_strlen(str) > 2) && (cm_strncmp(str, "0x", 2) == 0)) {
			base = 16;
		} else {
			base = 10;
		}
	}

	len = cm__scan_u64(str, base, &value);
	if (end_ptr) *end_ptr = (char *)str + len;
	return value;
}

i64 
cm_str_to_i64(char const *str, char **end_ptr, i32 base) {
	isize len;
	i64 value;

	if (!base) {
		if ((cm_strlen(str) > 2) && (cm_strncmp(str, "0x", 2) == 0)) {
			base = 16;
		} else {
			base = 10;
		}
	}

	len = cm__scan_i64(str, base, &value);
	if (end_ptr) *end_ptr = (char *)str + len;
	return value;
}


// TODO(bill): Are these good enough for characters?
cm_global char const 
cm__num_to_char_table[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"@$";

cm_inline void 
cm_i64_to_str(i64 value, char *string, i32 base) {
	char *buf = string;
	b32 negative = false;
	u64 v;
	if (value < 0) {
		negative = true;
		value = -value;
	}
	v = cast(u64)value;
	if (v != 0) {
		while (v > 0) {
			*buf++ = cm__num_to_char_table[v % base];
			v /= base;
		}
	} else {
		*buf++ = '0';
	}
	if (negative) {
		*buf++ = '-';
	}
	*buf = '\0';
	cm_strrev(string);
}


cm_inline void 
cm_u64_to_str(u64 value, char *string, i32 base) {
	char *buf = string;

	if (value) {
		while (value > 0) {
			*buf++ = cm__num_to_char_table[value % base];
			value /= base;
		}
	} else {
		*buf++ = '0';
	}
	*buf = '\0';

	cm_strrev(string);
}

cm_inline f32 
cm_str_to_f32(char const *str, char **end_ptr) {
	f64 f = cm_str_to_f64(str, end_ptr);
	f32 r = cast(f32)f;
	return r;
}


cm_inline f64 
cm_str_to_f64(char const *str, char **end_ptr) {
	f64 result, value, sign, scale;
	i32 frac;

	while (cm_char_is_space(*str)) {
		str++;
	}

	sign = 1.0;
	if (*str == '-') {
		sign = -1.0;
		str++;
	} else if (*str == '+') {
		str++;
	}

	for (value = 0.0; cm_char_is_digit(*str); str++) {
		value = value * 10.0 + (*str-'0');
	}

	if (*str == '.') {
		f64 pow10 = 10.0;
		str++;
		while (cm_char_is_digit(*str)) {
			value += (*str-'0') / pow10;
			pow10 *= 10.0;
			str++;
		}
	}

	frac = 0;
	scale = 1.0;
	if ((*str == 'e') || (*str == 'E')) {
		u32 exp;

		str++;
		if (*str == '-') {
			frac = 1;
			str++;
		} else if (*str == '+') {
			str++;
		}

		for (exp = 0; cm_char_is_digit(*str); str++) {
			exp = exp * 10 + (*str-'0');
		}
		if (exp > 308) exp = 308;

		while (exp >= 50) { scale *= 1e50; exp -= 50; }
		while (exp >=  8) { scale *= 1e8;  exp -=  8; }
		while (exp >   0) { scale *= 10.0; exp -=  1; }
	}

	result = sign * (frac ? (value / scale) : (value * scale));

	if (end_ptr) *end_ptr = cast(char *)str;

	return result;
}

CM_END_EXTERN