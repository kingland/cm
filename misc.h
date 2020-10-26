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

#ifndef CM_MISC_H
#define CM_MISC_H

#include "dll.h"
#include "types.h"

CM_BEGIN_EXTERN

CM_DEF void cm_exit     (u32 code);
CM_DEF void cm_yield    (void);
CM_DEF void cm_set_env  (char const *name, char const *value);
CM_DEF void cm_unset_env(char const *name);

CM_DEF u16 cm_endian_swap16(u16 i);
CM_DEF u32 cm_endian_swap32(u32 i);
CM_DEF u64 cm_endian_swap64(u64 i);

CM_DEF isize cm_count_set_bits(u64 mask);

CM_END_EXTERN

#endif
