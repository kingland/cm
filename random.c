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

#include "random.h"
#include "utils.h"
#include "time.h"
#include "atomics.h"
#include "thread.h"

CM_BEGIN_EXTERN

cm_global 
cmAtomic32 cm__random_shared_counter = {0};

cm_internal u32 
cm__get_noise_from_time(void) {
	u32 accum = 0;
	f64 start, remaining, end, curr = 0;
	u64 interval = 100000ll;

	start     = cm_time_now();
	remaining = (interval - cast(u64)(interval*start)%interval) / cast(f64)interval;
	end       = start + remaining;

	do {
		curr = cm_time_now();
		accum += cast(u32)curr;
	} while (curr >= end);
	return accum;
}

// NOTE(bill): Partly from http://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
// But the generation is even more random-er-est

cm_internal cm_inline u32 
cm__permute_qpr(u32 x) {
	cm_local_persist u32 const prime = 4294967291; // 2^32 - 5
	if (x >= prime) {
		return x;
	} else {
		u32 residue = cast(u32)(cast(u64) x * x) % prime;
		if (x <= prime / 2) {
			return residue;
		} else {
			return prime - residue;
		}
	}
}

cm_internal cm_inline u32 
cm__permute_with_offset(u32 x, u32 offset) {
	return (cm__permute_qpr(x) + offset) ^ 0x5bf03635;
}

void 
cm_random_init(cmRandom *r) {
	u64 time, tick;
	isize i, j;
	u32 x = 0;
	r->value = 0;

	r->offsets[0] = cm__get_noise_from_time();
	r->offsets[1] = cm_atomic32_fetch_add(&cm__random_shared_counter, 1);
	r->offsets[2] = cm_thread_current_id();
	r->offsets[3] = cm_thread_current_id() * 3 + 1;
	time = cm_utc_time_now();
	r->offsets[4] = cast(u32)(time >> 32);
	r->offsets[5] = cast(u32)time;
	r->offsets[6] = cm__get_noise_from_time();
	tick = cm_rdtsc();
	r->offsets[7] = cast(u32)(tick ^ (tick >> 32));

	for (j = 0; j < 4; j++) {
		for (i = 0; i < cm_count_of(r->offsets); i++) {
			r->offsets[i] = x = cm__permute_with_offset(x, r->offsets[i]);
		}
	}
}

u32 
cm_random_gen_u32(cmRandom *r) {
	u32 x = r->value;
	u32 carry = 1;
	isize i;
	for (i = 0; i < cm_count_of(r->offsets); i++) {
		x = cm__permute_with_offset(x, r->offsets[i]);
		if (carry > 0) {
			carry = ++r->offsets[i] ? 0 : 1;
		}
	}

	r->value = x;
	return x;
}

u32 
cm_random_gen_u32_unique(cmRandom *r) {
	u32 x = r->value;
	isize i;
	r->value++;
	for (i = 0; i < cm_count_of(r->offsets); i++) {
		x = cm__permute_with_offset(x, r->offsets[i]);
	}

	return x;
}

u64 
cm_random_gen_u64(cmRandom *r) {
	return ((cast(u64)cm_random_gen_u32(r)) << 32) | cm_random_gen_u32(r);
}

isize 
cm_random_gen_isize(cmRandom *r) {
	u64 u = cm_random_gen_u64(r);
	return *cast(isize *)&u;
}

i64 
cm_random_range_i64(cmRandom *r, i64 lower_inc, i64 higher_inc) {
	u64 u = cm_random_gen_u64(r);
	i64 i = *cast(i64 *)&u;
	i64 diff = higher_inc-lower_inc+1;
	i %= diff;
	i += lower_inc;
	return i;
}

isize 
cm_random_range_isize(cmRandom *r, isize lower_inc, isize higher_inc) {
	u64 u = cm_random_gen_u64(r);
	isize i = *cast(isize *)&u;
	isize diff = higher_inc-lower_inc+1;
	i %= diff;
	i += lower_inc;
	return i;
}

// NOTE(bill): Semi-cc'ed from cm_math to remove need for fmod and math.h
f64 
cm__copy_sign64(f64 x, f64 y) {
	i64 ix, iy;
	ix = *(i64 *)&x;
	iy = *(i64 *)&y;

	ix &= 0x7fffffffffffffff;
	ix |= iy & 0x8000000000000000;
	return *cast(f64 *)&ix;
}

f64 cm__floor64    (f64 x)        { return cast(f64)((x >= 0.0) ? cast(i64)x : cast(i64)(x-0.9999999999999999)); }
f64 cm__ceil64     (f64 x)        { return cast(f64)((x < 0) ? cast(i64)x : (cast(i64)x)+1); }
f64 cm__round64    (f64 x)        { return cast(f64)((x >= 0.0) ? cm__floor64(x + 0.5) : cm__ceil64(x - 0.5)); }
f64 cm__remainder64(f64 x, f64 y) { return x - (cm__round64(x/y)*y); }
f64 cm__abs64      (f64 x)        { return x < 0 ? -x : x; }
f64 cm__sign64     (f64 x)        { return x < 0 ? -1.0 : +1.0; }

f64 
cm__mod64(f64 x, f64 y) {
	f64 result;
	y = cm__abs64(y);
	result = cm__remainder64(cm__abs64(x), y);
	if (cm__sign64(result)) result += y;
	return cm__copy_sign64(result, x);
}

f64 
cm_random_range_f64(cmRandom *r, f64 lower_inc, f64 higher_inc) {
	u64 u = cm_random_gen_u64(r);
	f64 f = *cast(f64 *)&u;
	f64 diff = higher_inc-lower_inc+1.0;
	f = cm__mod64(f, diff);
	f += lower_inc;
	return f;
}


CM_END_EXTERN