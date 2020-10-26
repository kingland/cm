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

#ifndef CM_HASHTABLE_H
#define CM_HASHTABLE_H

#include "memory.h"
#include "dynarray.h"

CM_BEGIN_EXTERN

/////////////////////////////////////////////////////////////////////////////////
//
// Instantiated Hash Table
//
// This is an attempt to implement a templated hash table
// NOTE(bill): The key is aways a u64 for simplicity and you will _probably_ _never_ need anything bigger.
//
// Hash table type and function declaration, call: CM_TABLE_DECLARE(PREFIX, NAME, N, VALUE)
// Hash table function definitions, call: CM_TABLE_DEFINE(NAME, N, VALUE)
//
//     PREFIX  - a prefix for function prototypes e.g. extern, static, etc.
//     NAME    - Name of the Hash Table
//     FUNC    - the name will prefix function names
//     VALUE   - the type of the value to be stored
//
// NOTE(bill): I really wish C had decent metaprogramming capabilities (and no I don't mean C++'s templates either)
//
/////////////////////////////////////////////////////////////////////////////////

typedef struct cmHashTableFindResult {
	isize hash_index;
	isize entry_prev;
	isize entry_index;
} cmHashTableFindResult;

#define CM_TABLE(PREFIX, NAME, FUNC, VALUE) \
	CM_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE); \
	CM_TABLE_DEFINE(NAME, FUNC, VALUE);

#define CM_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE) \
typedef struct CM_JOIN2(NAME,Entry) { \
	u64 key; \
	isize next; \
	VALUE value; \
} CM_JOIN2(NAME,Entry); \
\
typedef struct NAME { \
	cmArray(isize) hashes; \
	cmArray(CM_JOIN2(NAME,Entry)) entries; \
} NAME; \
\
PREFIX void                  CM_JOIN2(FUNC,init)       (NAME *h, cmAllocator a); \
PREFIX void                  CM_JOIN2(FUNC,destroy)    (NAME *h); \
PREFIX VALUE *               CM_JOIN2(FUNC,get)        (NAME *h, u64 key); \
PREFIX void                  CM_JOIN2(FUNC,set)        (NAME *h, u64 key, VALUE value); \
PREFIX void                  CM_JOIN2(FUNC,grow)       (NAME *h); \
PREFIX void                  CM_JOIN2(FUNC,rehash)     (NAME *h, isize new_count); \

#define CM_TABLE_DEFINE(NAME, FUNC, VALUE) \
void CM_JOIN2(FUNC,init)(NAME *h, cmAllocator a) { \
	cm_array_init(h->hashes,  a); \
	cm_array_init(h->entries, a); \
} \
\
void CM_JOIN2(FUNC,destroy)(NAME *h) { \
	if (h->entries) cm_array_free(h->entries); \
	if (h->hashes)  cm_array_free(h->hashes); \
} \
\
cm_internal isize CM_JOIN2(FUNC,_add_entry)(NAME *h, u64 key) { \
	isize index; \
	CM_JOIN2(NAME,Entry) e = {0}; \
	e.key = key; \
	e.next = -1; \
	index = cm_array_count(h->entries); \
	cm_array_append(h->entries, e); \
	return index; \
} \
\
cm_internal cmHashTableFindResult CM_JOIN2(FUNC,_find)(NAME *h, u64 key) { \
	cmHashTableFindResult r = {-1, -1, -1}; \
	if (cm_array_count(h->hashes) > 0) { \
		r.hash_index  = key % cm_array_count(h->hashes); \
		r.entry_index = h->hashes[r.hash_index]; \
		while (r.entry_index >= 0) { \
			if (h->entries[r.entry_index].key == key) \
				return r; \
			r.entry_prev = r.entry_index; \
			r.entry_index = h->entries[r.entry_index].next; \
		} \
	} \
	return r; \
} \
\
cm_internal b32 CM_JOIN2(FUNC,_full)(NAME *h) { \
	return 0.75f * cm_array_count(h->hashes) < cm_array_count(h->entries); \
} \
\
void CM_JOIN2(FUNC,grow)(NAME *h) { \
	isize new_count = CM_ARRAY_GROW_FORMULA(cm_array_count(h->entries)); \
	CM_JOIN2(FUNC,rehash)(h, new_count); \
} \
\
void CM_JOIN2(FUNC,rehash)(NAME *h, isize new_count) { \
	isize i, j; \
	NAME nh = {0}; \
	CM_JOIN2(FUNC,init)(&nh, cm_array_allocator(h->hashes)); \
	cm_array_resize(nh.hashes, new_count); \
	cm_array_reserve(nh.entries, cm_array_count(h->entries)); \
	for (i = 0; i < new_count; i++) \
		nh.hashes[i] = -1; \
	for (i = 0; i < cm_array_count(h->entries); i++) { \
		CM_JOIN2(NAME,Entry) *e; \
		cmHashTableFindResult fr; \
		if (cm_array_count(nh.hashes) == 0) \
			CM_JOIN2(FUNC,grow)(&nh); \
		e = &h->entries[i]; \
		fr = CM_JOIN2(FUNC,_find)(&nh, e->key); \
		j = CM_JOIN2(FUNC,_add_entry)(&nh, e->key); \
		if (fr.entry_prev < 0) \
			nh.hashes[fr.hash_index] = j; \
		else \
			nh.entries[fr.entry_prev].next = j; \
		nh.entries[j].next = fr.entry_index; \
		nh.entries[j].value = e->value; \
		if (CM_JOIN2(FUNC,_full)(&nh)) \
			CM_JOIN2(FUNC,grow)(&nh); \
	} \
	CM_JOIN2(FUNC,destroy)(h); \
	h->hashes  = nh.hashes; \
	h->entries = nh.entries; \
} \
\
VALUE *CM_JOIN2(FUNC,get)(NAME *h, u64 key) { \
	isize index = CM_JOIN2(FUNC,_find)(h, key).entry_index; \
	if (index >= 0) \
		return &h->entries[index].value; \
	return NULL; \
} \
\
void CM_JOIN2(FUNC,set)(NAME *h, u64 key, VALUE value) { \
	isize index; \
	cmHashTableFindResult fr; \
	if (cm_array_count(h->hashes) == 0) \
		CM_JOIN2(FUNC,grow)(h); \
	fr = CM_JOIN2(FUNC,_find)(h, key); \
	if (fr.entry_index >= 0) { \
		index = fr.entry_index; \
	} else { \
		index = CM_JOIN2(FUNC,_add_entry)(h, key); \
		if (fr.entry_prev >= 0) { \
			h->entries[fr.entry_prev].next = index; \
		} else { \
			h->hashes[fr.hash_index] = index; \
		} \
	} \
	h->entries[index].value = value; \
	if (CM_JOIN2(FUNC,_full)(h)) \
		CM_JOIN2(FUNC,grow)(h); \
} \

CM_END_EXTERN

#endif