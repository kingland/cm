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

#ifndef CM_TIME_H
#define CM_TIME_H

#include "dll.h"
#include "types.h"

CM_BEGIN_EXTERN

////////////////////////////////////////////////////////////////
//
// Time
//
////////////////////////////////////////////////////////////////

CM_DEF u64  cm_rdtsc       (void);
CM_DEF f64  cm_time_now    (void); // NOTE(bill): This is only for relative time e.g. game loops
CM_DEF u64  cm_utc_time_now(void); // NOTE(bill): Number of microseconds since 1601-01-01 UTC
CM_DEF void cm_sleep_ms    (u32 ms);

CM_END_EXTERN

#endif //CM_TIME_H