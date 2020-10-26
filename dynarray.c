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

#include "dynarray.h"

cm_no_inline void *
cm__array_set_capacity(void *array, isize capacity, isize element_size) {

	cmArrayHeader *h = CM_ARRAY_HEADER(array);

	CM_ASSERT(element_size > 0);

	if (capacity == h->capacity)
		return array;

	if (capacity < h->count) {
		if (h->capacity < capacity) {
			isize new_capacity = CM_ARRAY_GROW_FORMULA(h->capacity);
			if (new_capacity < capacity)
				new_capacity = capacity;
			cm__array_set_capacity(array, new_capacity, element_size);
		}
		h->count = capacity;
	}

	{
		isize size = cm_size_of(cmArrayHeader) + element_size*capacity;
		cmArrayHeader *nh = cast(cmArrayHeader *)cm_alloc(h->allocator, size);
		cm_memmove(nh, h, cm_size_of(cmArrayHeader) + element_size*h->count);
		nh->allocator = h->allocator;
		nh->count     = h->count;
		nh->capacity  = capacity;
		cm_free(h->allocator, h);
		return nh+1;
	}
}
