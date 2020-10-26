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

#include "print.h"
#include "utils.h"
#include "char.h"

////////////////////////////////////////////////////////////////
//
// Printing
//
//

isize 
cm_printf(char const *fmt, ...) {
	isize res;
	va_list va;
	va_start(va, fmt);
	res = cm_printf_va(fmt, va);
	va_end(va);
	return res;
}

isize 
cm_printf_err(char const *fmt, ...) {
	isize res;
	va_list va;
	va_start(va, fmt);
	res = cm_printf_err_va(fmt, va);
	va_end(va);
	return res;
}

isize 
cm_fprintf(struct cmFile *f, char const *fmt, ...) {
	isize res;
	va_list va;
	va_start(va, fmt);
	res = cm_fprintf_va(f, fmt, va);
	va_end(va);
	return res;
}

char *
cm_bprintf(char const *fmt, ...) {
	va_list va;
	char *str;
	va_start(va, fmt);
	str = cm_bprintf_va(fmt, va);
	va_end(va);
	return str;
}

isize 
cm_snprintf(char *str, isize n, char const *fmt, ...) {
	isize res;
	va_list va;
	va_start(va, fmt);
	res = cm_snprintf_va(str, n, fmt, va);
	va_end(va);
	return res;
}

cm_inline isize 
cm_printf_va(char const *fmt, va_list va) {
	return cm_fprintf_va(cm_file_get_standard(cmFileStandard_Output), fmt, va);
}

cm_inline isize 
cm_printf_err_va(char const *fmt, va_list va) {
	return cm_fprintf_va(cm_file_get_standard(cmFileStandard_Error), fmt, va);
}

cm_inline isize 
cm_fprintf_va(struct cmFile *f, char const *fmt, va_list va) {
	cm_local_persist char buf[4096];
	isize len = cm_snprintf_va(buf, cm_size_of(buf), fmt, va);
	cm_file_write(f, buf, len-1); // NOTE(bill): prevent extra whitespace
	return len;
}

cm_inline char *
cm_bprintf_va(char const *fmt, va_list va) {
	cm_local_persist char buffer[4096];
	cm_snprintf_va(buffer, cm_size_of(buffer), fmt, va);
	return buffer;
}

enum {
	cmFmt_Minus     = CM_BIT(0),
	cmFmt_Plus      = CM_BIT(1),
	cmFmt_Alt       = CM_BIT(2),
	cmFmt_Space     = CM_BIT(3),
	cmFmt_Zero      = CM_BIT(4),

	cmFmt_Char      = CM_BIT(5),
	cmFmt_Short     = CM_BIT(6),
	cmFmt_Int       = CM_BIT(7),
	cmFmt_Long      = CM_BIT(8),
	cmFmt_Llong     = CM_BIT(9),
	cmFmt_Size      = CM_BIT(10),
	cmFmt_Intptr    = CM_BIT(11),

	cmFmt_Unsigned  = CM_BIT(12),
	cmFmt_Lower     = CM_BIT(13),
	cmFmt_Upper     = CM_BIT(14),


	cmFmt_Done      = CM_BIT(30),

	cmFmt_Ints = cmFmt_Char|cmFmt_Short|cmFmt_Int|cmFmt_Long|cmFmt_Llong|cmFmt_Size|cmFmt_Intptr
};

typedef struct {
	i32 base;
	i32 flags;
	i32 width;
	i32 precision;
} cmprivFmtInfo;


cm_internal isize 
cm__print_string(char *text, isize max_len, cmprivFmtInfo *info, char const *str) {
	// TODO(bill): Get precision and width to work correctly. How does it actually work?!
	// TODO(bill): This looks very buggy indeed.
	isize res = 0, len;
	isize remaining = max_len;

	if (info && info->precision >= 0) {
		len = cm_strnlen(str, info->precision);
	} else {
		len = cm_strlen(str);
	}

	if (info && (info->width == 0 || info->flags & cmFmt_Minus)) {
		if (info->precision > 0) {
			len = info->precision < len ? info->precision : len;
		}

		res += cm_strlcpy(text, str, len);

		if (info->width > res) {
			isize padding = info->width - len;
			char pad = (info->flags & cmFmt_Zero) ? '0' : ' ';
			while (padding --> 0 && remaining --> 0) {
				*text++ = pad, res++;
			}
		}
	} else {
		if (info && (info->width > res)) {
			isize padding = info->width - len;
			char pad = (info->flags & cmFmt_Zero) ? '0' : ' ';
			while (padding --> 0 && remaining --> 0) {
				*text++ = pad, res++;
			}
		}

		res += cm_strlcpy(text, str, len);
	}


	if (info) {
		if (info->flags & cmFmt_Upper) {
			cm_str_to_upper(text);
		} else if (info->flags & cmFmt_Lower) {
			cm_str_to_lower(text);
		}
	}

	return res;
}

cm_internal isize 
cm__print_char(char *text, isize max_len, cmprivFmtInfo *info, char arg) {
	char str[2] = "";
	str[0] = arg;
	return cm__print_string(text, max_len, info, str);
}

cm_internal isize 
cm__print_i64(char *text, isize max_len, cmprivFmtInfo *info, i64 value) {
	char num[130];
	cm_i64_to_str(value, num, info ? info->base : 10);
	return cm__print_string(text, max_len, info, num);
}

cm_internal isize 
cm__print_u64(char *text, isize max_len, cmprivFmtInfo *info, u64 value) {
	char num[130];
	cm_u64_to_str(value, num, info ? info->base : 10);
	return cm__print_string(text, max_len, info, num);
}


cm_internal isize 
cm__print_f64(char *text, isize max_len, cmprivFmtInfo *info, f64 arg) {
	// TODO(bill): Handle exponent notation
	isize width, len, remaining = max_len;
	char *text_begin = text;

	if (arg) {
		u64 value;
		if (arg < 0) {
			if (remaining > 1) {
				*text = '-', remaining--;
			}
			text++;
			arg = -arg;
		} else if (info->flags & cmFmt_Minus) {
			if (remaining > 1) {
				*text = '+', remaining--;
			}
			text++;
		}

		value = cast(u64)arg;
		len = cm__print_u64(text, remaining, NULL, value);
		text += len;

		if (len >= remaining) {
			remaining = CM_MIN(remaining, 1);
		} else {
			remaining -= len;
		}
		arg -= value;

		if (info->precision < 0) {
			info->precision = 6;
		}

		if ((info->flags & cmFmt_Alt) || info->precision > 0) {
			i64 mult = 10;
			if (remaining > 1) {
				*text = '.', remaining--;
			}
			text++;
			while (info->precision-- > 0) {
				value = cast(u64)(arg * mult);
				len = cm__print_u64(text, remaining, NULL, value);
				text += len;
				if (len >= remaining) {
					remaining = CM_MIN(remaining, 1);
				} else {
					remaining -= len;
				}
				arg -= cast(f64)value / mult;
				mult *= 10;
			}
		}
	} else {
		if (remaining > 1) {
			*text = '0', remaining--;
		}
		text++;
		if (info->flags & cmFmt_Alt) {
			if (remaining > 1) {
				*text = '.', remaining--;
			}
			text++;
		}
	}

	width = info->width - (text - text_begin);
	if (width > 0) {
		char fill = (info->flags & cmFmt_Zero) ? '0' : ' ';
		char *end = text+remaining-1;
		len = (text - text_begin);

		for (len = (text - text_begin); len--; ) {
			if ((text_begin+len+width) < end) {
				*(text_begin+len+width) = *(text_begin+len);
			}
		}

		len = width;
		text += len;
		if (len >= remaining) {
			remaining = CM_MIN(remaining, 1);
		} else {
			remaining -= len;
		}

		while (len--) {
			if (text_begin+len < end) {
				text_begin[len] = fill;
			}
		}
	}

	return (text - text_begin);
}

cm_no_inline isize 
cm_snprintf_va(char *text, isize max_len, char const *fmt, va_list va) {
	char const *text_begin = text;
	isize remaining = max_len, res;

	while (*fmt) {
		cmprivFmtInfo info = {0};
		isize len = 0;
		info.precision = -1;

		while (*fmt && *fmt != '%' && remaining) {
			*text++ = *fmt++;
		}

		if (*fmt == '%') {
			do {
				switch (*++fmt) {
				case '-': info.flags |= cmFmt_Minus; break;
				case '+': info.flags |= cmFmt_Plus;  break;
				case '#': info.flags |= cmFmt_Alt;   break;
				case ' ': info.flags |= cmFmt_Space; break;
				case '0': info.flags |= cmFmt_Zero;  break;
				default:  info.flags |= cmFmt_Done;  break;
				}
			} while (!(info.flags & cmFmt_Done));
		}

		// NOTE(bill): Optional Width
		if (*fmt == '*') {
			int width = va_arg(va, int);
			if (width < 0) {
				info.flags |= cmFmt_Minus;
				info.width = -width;
			} else {
				info.width = width;
			}
			fmt++;
		} else {
			info.width = cast(i32)cm_str_to_i64(fmt, cast(char **)&fmt, 10);
		}

		// NOTE(bill): Optional Precision
		if (*fmt == '.') {
			fmt++;
			if (*fmt == '*') {
				info.precision = va_arg(va, int);
				fmt++;
			} else {
				info.precision = cast(i32)cm_str_to_i64(fmt, cast(char **)&fmt, 10);
			}
			info.flags &= ~cmFmt_Zero;
		}


		switch (*fmt++) {
		case 'h':
			if (*fmt == 'h') { // hh => char
				info.flags |= cmFmt_Char;
				fmt++;
			} else { // h => short
				info.flags |= cmFmt_Short;
			}
			break;

		case 'l':
			if (*fmt == 'l') { // ll => long long
				info.flags |= cmFmt_Llong;
				fmt++;
			} else { // l => long
				info.flags |= cmFmt_Long;
			}
			break;

			break;

		case 'z': // NOTE(bill): usize
			info.flags |= cmFmt_Unsigned;
			// fallthrough
		case 't': // NOTE(bill): isize
			info.flags |= cmFmt_Size;
			break;

		default: fmt--; break;
		}


		switch (*fmt) {
		case 'u':
			info.flags |= cmFmt_Unsigned;
			// fallthrough
		case 'd':
		case 'i':
			info.base = 10;
			break;

		case 'o':
			info.base = 8;
			break;

		case 'x':
			info.base = 16;
			info.flags |= (cmFmt_Unsigned | cmFmt_Lower);
			break;

		case 'X':
			info.base = 16;
			info.flags |= (cmFmt_Unsigned | cmFmt_Upper);
			break;

		case 'f':
		case 'F':
		case 'g':
		case 'G':
			len = cm__print_f64(text, remaining, &info, va_arg(va, f64));
			break;

		case 'a':
		case 'A':
			// TODO(bill):
			break;

		case 'c':
			len = cm__print_char(text, remaining, &info, cast(char)va_arg(va, int));
			break;

		case 's':
			len = cm__print_string(text, remaining, &info, va_arg(va, char *));
			break;

		case 'p':
			info.base = 16;
			info.flags |= (cmFmt_Lower|cmFmt_Unsigned|cmFmt_Alt|cmFmt_Intptr);
			break;

		case '%':
			len = cm__print_char(text, remaining, &info, '%');
			break;

		default: fmt--; break;
		}

		fmt++;

		if (info.base != 0) {
			if (info.flags & cmFmt_Unsigned) {
				u64 value = 0;
				switch (info.flags & cmFmt_Ints) {
				case cmFmt_Char:   value = cast(u64)cast(u8) va_arg(va, int);       break;
				case cmFmt_Short:  value = cast(u64)cast(u16)va_arg(va, int);       break;
				case cmFmt_Long:   value = cast(u64)va_arg(va, unsigned long);      break;
				case cmFmt_Llong:  value = cast(u64)va_arg(va, unsigned long long); break;
				case cmFmt_Size:   value = cast(u64)va_arg(va, usize);              break;
				case cmFmt_Intptr: value = cast(u64)va_arg(va, uintptr);            break;
				default:             value = cast(u64)va_arg(va, unsigned int);       break;
				}

				len = cm__print_u64(text, remaining, &info, value);

			} else {
				i64 value = 0;
				switch (info.flags & cmFmt_Ints) {
				case cmFmt_Char:   value = cast(i64)cast(i8) va_arg(va, int); break;
				case cmFmt_Short:  value = cast(i64)cast(i16)va_arg(va, int); break;
				case cmFmt_Long:   value = cast(i64)va_arg(va, long);         break;
				case cmFmt_Llong:  value = cast(i64)va_arg(va, long long);    break;
				case cmFmt_Size:   value = cast(i64)va_arg(va, usize);        break;
				case cmFmt_Intptr: value = cast(i64)va_arg(va, uintptr);      break;
				default:             value = cast(i64)va_arg(va, int);          break;
				}

				len = cm__print_i64(text, remaining, &info, value);
			}
		}


		text += len;
		if (len >= remaining) {
			remaining = CM_MIN(remaining, 1);
		} else {
			remaining -= len;
		}
	}

	*text++ = '\0';
	res = (text - text_begin);
	return (res >= max_len || res < 0) ? -1 : res;
}