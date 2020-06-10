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

#include "string.h"
#include "utils.h"
#include "header.h"
#include "memory.h"
#include "char.h"
#include "utf8.h"
#include "print.h"

CM_BEGIN_EXTERN

cm_inline void 
cm__set_string_length  (cmString str, isize len) { CM_STRING_HEADER(str)->length = len; }

cm_inline void 
cm__set_string_capacity(cmString str, isize cap) { CM_STRING_HEADER(str)->capacity = cap; }

cmString 
cm_string_make_reserve(cmAllocator a, isize capacity) {
	isize header_size = cm_size_of(cmStringHeader);
	void *ptr = cm_alloc(a, header_size + capacity + 1);

	cmString str;
	cmStringHeader *header;

	if (ptr == NULL) return NULL;
	cm_zero_size(ptr, header_size + capacity + 1);

	str = cast(char *)ptr + header_size;
	header = CM_STRING_HEADER(str);
	header->allocator = a;
	header->length    = 0;
	header->capacity  = capacity;
	str[capacity] = '\0';

	return str;
}

cm_inline cmString 
cm_string_make(cmAllocator a, char const *str) {
	isize len = str ? cm_strlen(str) : 0;
	return cm_string_make_length(a, str, len);
}

cmString 
cm_string_make_length(cmAllocator a, void const *init_str, isize num_bytes) {
	isize header_size = cm_size_of(cmStringHeader);
	void *ptr = cm_alloc(a, header_size + num_bytes + 1);

	cmString str;
	cmStringHeader *header;

	if (ptr == NULL) return NULL;
	if (!init_str) cm_zero_size(ptr, header_size + num_bytes + 1);

	str = cast(char *)ptr + header_size;
	header = CM_STRING_HEADER(str);
	header->allocator = a;
	header->length    = num_bytes;
	header->capacity  = num_bytes;
	if (num_bytes && init_str) {
		cm_memcopy(str, init_str, num_bytes);
	}
	str[num_bytes] = '\0';

	return str;
}

cm_inline void 
cm_string_free(cmString str) {
	if (str) {
		cmStringHeader *header = CM_STRING_HEADER(str);
		cm_free(header->allocator, header);
	}

}

cm_inline cmString 
cm_string_duplicate(cmAllocator a, cmString const str) { 
	return cm_string_make_length(a, str, cm_string_length(str)); 
}

cm_inline isize 
cm_string_length  (cmString const str) { 
	return CM_STRING_HEADER(str)->length; 
}

cm_inline isize 
cm_string_capacity(cmString const str) { 
	return CM_STRING_HEADER(str)->capacity; 
}

cm_inline isize 
cm_string_available_space(cmString const str) {
	cmStringHeader *h = CM_STRING_HEADER(str);
	if (h->capacity > h->length) {
		return h->capacity - h->length;
	}
	return 0;
}

cm_inline void 
cm_string_clear(cmString str) { 
	cm__set_string_length(str, 0); str[0] = '\0'; 
}

cm_inline cmString 
cm_string_append(cmString str, cmString const other) { 
	return cm_string_append_length(str, other, cm_string_length(other)); 
}

cmString 
cm_string_append_length(cmString str, void const *other, isize other_len) {
	if (other_len > 0) {
		isize curr_len = cm_string_length(str);

		str = cm_string_make_space_for(str, other_len);
		if (str == NULL) {
			return NULL;
		}

		cm_memcopy(str + curr_len, other, other_len);
		str[curr_len + other_len] = '\0';
		cm__set_string_length(str, curr_len + other_len);
	}
	return str;
}

cm_inline cmString 
cm_string_append_char(cmString str, char const *other) {
	return cm_string_append_length(str, other, cm_strlen(other));
}

cmString 
cm_string_append_rune(cmString str, Rune r) {
	if (r >= 0) {
		u8 buf[8] = {0};
		isize len = cm_utf8_encode_rune(buf, r);
		return cm_string_append_length(str, buf, len);
	}
	return str;
}

cmString 
cm_string_append_fmt(cmString str, char const *fmt, ...) {
	isize res;
	char buf[4096] = {0};
	va_list va;
	va_start(va, fmt);
	res = cm_snprintf_va(buf, cm_count_of(buf)-1, fmt, va)-1;
	va_end(va);
	return cm_string_append_length(str, buf, res);
}

cmString 
cm_string_set(cmString str, char const *cstr) {
	isize len = cm_strlen(cstr);
	if (cm_string_capacity(str) < len) {
		str = cm_string_make_space_for(str, len - cm_string_length(str));
		if (str == NULL) {
			return NULL;
		}
	}

	cm_memcopy(str, cstr, len);
	str[len] = '\0';
	cm__set_string_length(str, len);

	return str;
}

cmString 
cm_string_make_space_for(cmString str, isize add_len) {
	isize available = cm_string_available_space(str);

	// NOTE(bill): Return if there is enough space left
	if (available >= add_len) {
		return str;
	} else {
		isize new_len, old_size, new_size;
		void *ptr, *new_ptr;
		cmAllocator a = CM_STRING_HEADER(str)->allocator;
		cmStringHeader *header;

		new_len = cm_string_length(str) + add_len;
		ptr = CM_STRING_HEADER(str);
		old_size = cm_size_of(cmStringHeader) + cm_string_length(str) + 1;
		new_size = cm_size_of(cmStringHeader) + new_len + 1;

		new_ptr = cm_resize(a, ptr, old_size, new_size);
		if (new_ptr == NULL) return NULL;

		header = cast(cmStringHeader *)new_ptr;
		header->allocator = a;

		str = cast(cmString)(header+1);
		cm__set_string_capacity(str, new_len);

		return str;
	}
}

cm_inline isize 
cm_string_allocation_size(cmString const str) {
	isize cap = cm_string_capacity(str);
	return cm_size_of(cmStringHeader) + cap;
}

cm_inline b32 
cm_string_are_equal(cmString const lhs, cmString const rhs) {
	isize lhs_len, rhs_len, i;
	lhs_len = cm_string_length(lhs);
	rhs_len = cm_string_length(rhs);
	if (lhs_len != rhs_len) {
		return false;
	}

	for (i = 0; i < lhs_len; i++) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

cmString 
cm_string_trim(cmString str, char const *cut_set) {
	char *start, *end, *start_pos, *end_pos;
	isize len;

	start_pos = start = str;
	end_pos   = end   = str + cm_string_length(str) - 1;

	while (start_pos <= end && cm_char_first_occurence(cut_set, *start_pos)) {
		start_pos++;
	}
	while (end_pos > start_pos && cm_char_first_occurence(cut_set, *end_pos)) {
		end_pos--;
	}

	len = cast(isize)((start_pos > end_pos) ? 0 : ((end_pos - start_pos)+1));

	if (str != start_pos)
		cm_memmove(str, start_pos, len);
	str[len] = '\0';

	cm__set_string_length(str, len);

	return str;
}

cm_inline cmString 
cm_string_trim_space(cmString str) { return cm_string_trim(str, " \t\r\n\v\f"); }

CM_END_EXTERN