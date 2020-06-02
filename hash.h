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

#ifndef CM_HASH_H
#define CM_HASH_H

#include "dll.h"
#include "types.h"

//
// Hashing and Checksum Functions
//

CM_EXTERN u32 cm_adler32(void const *data, isize len);

CM_EXTERN u32 cm_crc32(void const *data, isize len);
CM_EXTERN u64 cm_crc64(void const *data, isize len);

CM_EXTERN u32 cm_fnv32 (void const *data, isize len);
CM_EXTERN u64 cm_fnv64 (void const *data, isize len);
CM_EXTERN u32 cm_fnv32a(void const *data, isize len);
CM_EXTERN u64 cm_fnv64a(void const *data, isize len);

// NOTE(bill): Default seed of 0x9747b28c
// NOTE(bill): I prefer using murmur64 for most hashes
CM_EXTERN u32 cm_murmur32(void const *data, isize len);
CM_EXTERN u64 cm_murmur64(void const *data, isize len);

CM_EXTERN u32 cm_murmur32_seed(void const *data, isize len, u32 seed);
CM_EXTERN u64 cm_murmur64_seed(void const *data, isize len, u64 seed);

#endif