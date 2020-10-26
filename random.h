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

#ifndef CM_RANDOM_H
#define CM_RANDOM_H

#include "dll.h"
#include "types.h"

CM_BEGIN_EXTERN

////////////////////////////////////////////////////////////////
//
// Miscellany
//
////////////////////////////////////////////////////////////////

typedef struct cmRandom {
	u32 offsets[8];
	u32 value;
} cmRandom;

// NOTE(bill): Generates from numerous sources to produce a decent pseudo-random seed
CM_DEF void  cm_random_init          (cmRandom *r);
CM_DEF u32   cm_random_gen_u32       (cmRandom *r);
CM_DEF u32   cm_random_gen_u32_unique(cmRandom *r);
CM_DEF u64   cm_random_gen_u64       (cmRandom *r); // NOTE(bill): (gb_random_gen_u32() << 32) | gb_random_gen_u32()
CM_DEF isize cm_random_gen_isize     (cmRandom *r);
CM_DEF i64   cm_random_range_i64     (cmRandom *r, i64 lower_inc, i64 higher_inc);
CM_DEF isize cm_random_range_isize   (cmRandom *r, isize lower_inc, isize higher_inc);
CM_DEF f64   cm_random_range_f64     (cmRandom *r, f64 lower_inc, f64 higher_inc);

CM_END_EXTERN

#endif //CM_RANDOM_H