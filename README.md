## types.h
```c
typedef u8;
typedef i8;
typedef u16;
typedef i16;
typedef u32;
typedef i32;
typedef u64;
typedef i64;
typedef usize; //typedef unsigned long long size_t
typedef isize; //typedef long long ptrdiff_t
typedef intptr;
typedef uintptr;
typedef f32;
typedef f64;
typedef Rune;
typedef b8;
typedef b16;
typedef b32;
#define CM_RUNE_INVALID
#define CM_RUNE_MAX 
#define CM_RUNE_BOM 
#define CM_RUNE_EOF
#define U8_MIN
#define U8_MAX
#define I8_MIN
#define I8_MAX
#define U16_MIN
#define U16_MAX
#define I16_MIN
#define I16_MAX
#define U32_MIN
#define U32_MAX
#define I32_MIN
#define I32_MAX
#define U64_MIN
#define U64_MAX
#define I64_MIN
#define I64_MAX
#define USIZE_MIX 
#define USIZE_MAX
#define ISIZE_MIX
#define ISIZE_MAX
#define F32_MIN
#define F32_MAX
#define F64_MIN
#define F64_MAX
```
## arch.h
```c
/* 
 * Arch 
 */
#define CM_ARCH_64
#define CM_ARCH_32
/* 
 * Endian 
 */
#define CM_ENDIAN_ORDER
#define CM_BIG_ENDIAN
#define CM_LITTLE_ENDIAN
/*
 * OS
 */
#define CM_WINDOWS
#define CM_OS_WINDOWS
#define CM_SYS_WINDOWS
#define CM_OSX
#define CM_OS_OSX
#define CM_SYS_OSX
#define CM_UNIX
#define CM_OS_UNIX
#define CM_SYS_UNIX
#define CM_LINUX
#define CM_OS_LINUX
#define CM_SYS_LINUX
#define CM_FREEBSD
#define CM_OS_FREEBSD
#define CM_SYS_FREEBSD
/*
 * Compiler
 */
#define CM_COMPILER_MSVC
#define CM_COMPILER_GCC
#define CM_COMPILER_CLANG
/*
 * CPU
 */
#define CM_CPU_X86
#define CM_CPU_PPC
#define CM_CPU_ARM
#define CM_CPU_MIPS
#define CM_CACHE_LINE_SIZE
```
## assert.h
```c
#define CM_STATIC_ASSERT(cond) 
#define CM_STATIC_ASSERT1(cond, line)
#define CM_STATIC_ASSERT2(cond, line)
#define CM_STATIC_ASSERT3(cond, msg)
```
## affinity.h
- **Struct** 
```c
/*
 * cmAffinity
 */
typedef struct cmAffinity{
  b32   is_accurate;
  isize core_count;
  isize thread_count;
  usize core_masks[CM_WIN32_MAX_THREADS];
} cmAffinity;
```
- **Function**
```c
/*
 */
void cm_affinity_init (cmAffinity *a);
/*
 */
void cm_affinity_destroy (cmAffinity *a);
/*
 */
b32 cm_affinity_set (cmAffinity *a, isize core, isize thread);
/*
 */
isize cm_affinity_thread_count_for_core (cmAffinity *a, isize core);
```
## atomics.h
- **Struct** 
```c
/*
 * cmAtomic32
 */
typedef struct cmAtomic32  { 
  i32   volatile value; 
} cmAtomic32;
/*
 * cmAtomic64
 */
typedef struct cmAtomic64  {
  i64   volatile value; 
} cmAtomic64;
/*
 * cmAtomicPtr
 */
typedef struct cmAtomicPtr { 
  void *volatile value;
} cmAtomicPtr;
```
- **Function**
```c
/*
 * atomic32
 */
i32 cm_atomic32_load (cmAtomic32 const volatile *a);
void cm_atomic32_store (cmAtomic32 volatile *a, i32 value);
i32 cm_atomic32_compare_exchange (cmAtomic32 volatile *a, i32 expected, i32 desired);
i32 cm_atomic32_exchanged (cmAtomic32 volatile *a, i32 desired);
i32 cm_atomic32_fetch_add (cmAtomic32 volatile *a, i32 operand);
i32 cm_atomic32_fetch_and (cmAtomic32 volatile *a, i32 operand);
i32 cm_atomic32_fetch_or (cmAtomic32 volatile *a, i32 operand);
b32 cm_atomic32_spin_lock (cmAtomic32 volatile *a, isize time_out);
void cm_atomic32_spin_unlock (cmAtomic32 volatile *a);
b32 cm_atomic32_try_acquire_lock (cmAtomic32 volatile *a);
/*
 * atomic64
 */
i64 cm_atomic64_load (cmAtomic64 const volatile *a);
void cm_atomic64_store (cmAtomic64 volatile *a, i64 value);
i64 cm_atomic64_compare_exchange (cmAtomic64 volatile *a, i64 expected, i64 desired);
i64 cm_atomic64_exchanged (cmAtomic64 volatile *a, i64 desired);
i64 cm_atomic64_fetch_add (cmAtomic64 volatile *a, i64 operand);
i64 cm_atomic64_fetch_and (cmAtomic64 volatile *a, i64 operand);
i64 cm_atomic64_fetch_or (cmAtomic64 volatile *a, i64 operand);
b32 cm_atomic64_spin_lock (cmAtomic64 volatile *a, isize time_out);
void cm_atomic64_spin_unlock (cmAtomic64 volatile *a);
b32 cm_atomic64_try_acquire_lock (cmAtomic64 volatile *a);
/*
 * atomicptr
 */
void *cm_atomic_ptr_load (cmAtomicPtr const volatile *a);
void cm_atomic_ptr_store (cmAtomicPtr volatile *a, void *value);
void *cm_atomic_ptr_compare_exchange (cmAtomicPtr volatile *a, void *expected, void *desired);
void *cm_atomic_ptr_exchanged (cmAtomicPtr volatile *a, void *desired);
void *cm_atomic_ptr_fetch_add (cmAtomicPtr volatile *a, void *operand);
void *cm_atomic_ptr_fetch_and (cmAtomicPtr volatile *a, void *operand);
void *cm_atomic_ptr_fetch_or (cmAtomicPtr volatile *a, void *operand);
b32 cm_atomic_ptr_spin_lock (cmAtomicPtr volatile *a, isize time_out);
void cm_atomic_ptr_spin_unlock (cmAtomicPtr volatile *a);
b32 cm_atomic_ptr_try_acquire_lock (cmAtomicPtr volatile *a);
```
## buffer.h
- **Struct**
```c
/*
 * cmBufferHeader
 */
typedef struct cmBufferHeader {
  isize count;
  isize capacity;
} cmBufferHeader;
```
- **Function**
```c
/*
 * ประกาศตัวแปร Ex. cmBuffer(i32) buffer;
 */
cmBuffer(Type);
/*
 * จำนวน
 */
isize cm_buffer_count (cmBufferHeader * bfh);
/*
 * ความจุ
 */
isize cm_buffer_capacity (cmBufferHeader * bfh);
/*
 * ค่าเริ่มต้น
 */
void *cm_buffer_init (x, allocator, cap);
/*
 * เลิกใช้งาน
 */
void cm_buffer_free (x, allocator);
/*
 * เพิ่มข้อมูล(1 รายการ)
 */
void cm_buffer_append (x, item);
/*
 * เพิ่มข้อมูล(มากกว่า 1 รายการ)
 */
void cm_buffer_appendv (x, items, item_count);
/*
 * นำข้อมูลออก
 */
void cm_buffer_pop (x);
/*
 * ล้างข้อมูล
 */
cm_buffer_clear (x)
```
## char.h
- **Function**
```c
/*
 * แปลงตัวอักษรเป็นพิมพ์เล็ก
 */
char cm_char_to_lower (char c);
/*
 * แปลงตัวอักษรเป็นพิมพ์ใหญ่
 */
char cm_char_to_upper (char c);
/*
 * ตรวจสอบช่องว่าง
 */
b32 cm_char_is_space (char c);
/*
 * ตรวจสอบตัวเลข
 */
b32 cm_char_is_digit (char c);
/*
 * ตรวจสอบตัวเลขที่เป็นฐาน16 (hex)
 */
b32 cm_char_is_hex_digit (char c);
/*
 * 
 */
b32 cm_char_is_alpha (char c);
/*
 * ตรวจสอบว่าเป็นตัวเลขหรือตัวอักษร
 */
b32 cm_char_is_alphanumeric (char c);
/*
 * แปลงตัวอักษร
 */
i32 cm_digit_to_int (char c);
 /*
  * แปลงตัวอักษรฐาน16(hex)เป็นตัวเลขจำนวนเต็ม
  */
i32 cm_hex_digit_to_int (char c);
/*
 * แปลงตัวอักษรเป็นพิมพ์เล็ก
 */
void cm_str_to_lower(char *str);
/*
 * แปลงตัวอักษรเป็นพิมพ์ใหญ่
 */
void cm_str_to_upper(char *str);
/*
 * จำนวนข้อความ
 */
isize cm_strlen (char const *str);
/*
 * 
 */
isize cm_strnlen(char const *str, isize max_len);
/*
 * เปรียบเทียบข้อความ
 */
i32 cm_strcmp (char const *s1, char const *s2);
/*
 * 
 */
i32 cm_strncmp(char const *s1, char const *s2, isize len);
/*
 * ก๊อบปี้ข้อความ
 */
char *cm_strcpy (char *dest, char const *source);
/*
 * 
 */ 
char *cm_strncpy(char *dest, char const *source, isize len);
/*
 * 
 */
isize cm_strlcpy(char *dest, char const *source, isize len);
/*
 * 
 */
char *cm_strrev (char *str);
/*
 * 
 */
char const *cm_strtok(char *output, char const *src, char const *delimit);
/*
 * 
 */
b32 cm_str_has_prefix(char const *str, char const *prefix);
/*
 * 
 */
b32 cm_str_has_suffix(char const *str, char const *suffix);
/*
 * 
 */
char const *cm_char_first_occurence(char const *str, char c);
/*
 * 
 */
char const *cm_char_last_occurence (char const *str, char c);
/*
 * 
 */
void cm_str_concat(char *dest, isize dest_len, char const *src_a, isize src_a_len,  char const *src_b, isize src_b_len);
/*
 * 
 */
u64 cm_str_to_u64(char const *str, char **end_ptr, i32 base);
/*
 * 
 */
i64 cm_str_to_i64(char const *str, char **end_ptr, i32 base);
/*
 * 
 */
f32 cm_str_to_f32(char const *str, char **end_ptr);
/*
 * 
 */
f64 cm_str_to_f64(char const *str, char **end_ptr);
/*
 * 
 */
void cm_i64_to_str(i64 value, char *string, i32 base);
/*
 * 
 */
void cm_u64_to_str(u64 value, char *string, i32 base);
```
## debug.h
```c
#define CM_DEBUG_TRAP()
#define CM_ASSERT_MSG(cond, msg, ...)
#define CM_ASSERT(cond)
#define CM_ASSERT_NOT_NULL(ptr)
#define CM_PANIC(msg, ...)
```
- **Function**
```c
//
void cm_assert_handler(char const *prefix, char const *condition, char const *file, i32 line, char const *msg, ...);
```
## defer.h
```c
#define CM_DEFER_1(x, y)
#define CM_DEFER_2(x, y)
#define CM_DEFER_3(x)
```
- **Function**
```c
/*
ตัวอย่าง
cmMutex m;
cm_mutex_init(&m);
{
	cm_mutex_lock(&m);
	defer (cm_mutex_unlock(&m));

	...
}
*/
void defer(code);
```
## dll.h
```c
#define CM_DLL_EXPORT
#define CM_DLL_IMPORT
#define CM_DEF
#define CM_SHARED_LIB_BUILD
#define CM_SHARED_LIB_USE
#define CM_C_API
```
- **Struct**
```c
typedef void *cmDllHandle;
typedef void (*cmDllProc)(void);
```
- **Function**
```c
/*
 * Ex. cmDllHandle handle = cm_dll_load("opengl32.dll");
 */
cmDllHandle cm_dll_load (char const *filepath);
/*
 * Ex. cm_dll_unload(handle);
 */
void cm_dll_unload (cmDllHandle dll);
/*
 * Ex.
 * cmDllHandle xinput_library = cm_dll_load("xinput9_1_0.dll");
 * cmXInputGetStateProc *get_state = cast(cmXInputGetStateProc *)cm_dll_proc_address(xinput_library, "XInputGetState");
 * cmXInputGetStateProc *set_state = cast(cmXInputSetStateProc *)cm_dll_proc_address(xinput_library, "XInputSetState");
 */
cmDllProc cm_dll_proc_address (cmDllHandle dll, char const *proc_name);
```
## dynarray.h
```c
#define CM_ARRAY_GROW_FORMULA(x)
#define CM_ARRAY_HEADER(x)
```
- **Struct**
```c
/*
 * cmArrayHeader
 */
typedef struct cmArrayHeader {
  cmAllocator allocator;
  isize       count;
  isize       capacity;
} cmArrayHeader;
```
- **Function**
```c
/*
 * ประกาศตัวแปร Ex. cmArray(i32) array;
 */
cmArray(Type);
/*
 */
cmAllocator cm_array_allocator(x);
/*
 */
isize cm_array_count(x);
/*
 */
isize cm_array_capacity(x);
/*
 */
void cm_array_init_reserve(x, allocator_, cap);
/*
 */
void cm_array_init(x, allocator);
/*
 */
void cm_array_free(x);
/*
 */
void cm_array_set_capacity(x, capacity);
/*
 */
void *cm__array_set_capacity(void *array, isize capacity, isize element_size);
/*
 */
void cm_array_grow(x, min_capacity) ;
/*
 */
void cm_array_append(x, item);
/*
 */
void cm_array_appendv(x, items, item_count);
/*
 */
void cm_array_pop(x);
/*
 */
void cm_array_clear(x);
/*
 */
void cm_array_resize(x, new_count);
/*
 */
void cm_array_reserve(x, new_capacity);
```
## extern.h
```c
#define CM_EXTERN
#define CM_BEGIN_EXTERN
#define CM_END_EXTERN
```
## fences.h
- **Function**
```c
/*
 */
void cm_yield_thread(void);
/*
 */
void cm_mfence (void);
/*
 */
void cm_sfence (void);
/*
 */
void cm_lfence (void);
```
## file.h
- **Struct**
```c
/*
 */
typedef u32 cmFileMode;
/*
 */
typedef enum cmFileModeFlag {
  cmFileMode_Read,
  cmFileMode_Write,
  cmFileMode_Append,
  cmFileMode_Rw,
  cmFileMode_Modes ,
} cmFileModeFlag;
/*
 */
typedef enum cmSeekWhenceType {
  cmSeekWhence_Begin,
  cmSeekWhence_Current,
  cmSeekWhence_End,
} cmSeekWhenceType;
/*
 */
typedef enum cmFileError {
  cmFileError_None,
  cmFileError_Invalid,
  cmFileError_InvalidFilename,
  cmFileError_Exists,
  cmFileError_NotExists,
  cmFileError_Permission,
  cmFileError_TruncationFailure,
} cmFileError;
/*
 */
typedef union cmFileDescriptor {
  void *  p;
  intptr  i;
  uintptr u;
} cmFileDescriptor;
/*
 */
typedef struct cmFileOperations cmFileOperations;
/*
 */
typedef CM_FILE_OPEN_PROC(cmFileOpenProc);
/*
 */
typedef CM_FILE_READ_AT_PROC(cmFileReadProc);
/*
 */
typedef CM_FILE_WRITE_AT_PROC(cmFileWriteProc);
/*
 */
typedef CM_FILE_SEEK_PROC(cmFileSeekProc);
/*
 */
typedef CM_FILE_CLOSE_PROC(cmFileCloseProc)
/*
 */
struct cmFileOperations {
  cmFileReadProc  *read_at;
  cmFileWriteProc *write_at;
  cmFileSeekProc  *seek;
  cmFileCloseProc *close;
};
/*
 */
extern cmFileOperations const cmDefaultFileOperations;
/*
 */
typedef u64 cmFileTime;
/*
 */
typedef struct cmFile {
  cmFileOperations ops;
  cmFileDescriptor fd;
  char const *     filename;
  cmFileTime       last_write_time;
  // gbDirInfo *   dir_info; // TODO(bill): Get directory info
} cmFile;
/*
 */
typedef enum cmFileStandardType {
  cmFileStandard_Input,
  cmFileStandard_Output,
  cmFileStandard_Error,
  cmFileStandard_Count,
} cmFileStandardType;
/*
 */
typedef struct cmFileContents {
  cmAllocator allocator;
  void *      data;
  isize       size;
} cmFileContents;
/*
 */
#define CM_PATH_SEPARATOR
```
- **Function**
```c
/*
 */
cmFile *const cm_file_get_standard (cmFileStandardType std);
/*
 */
cmFileError cm_file_create (cmFile *file, char const *filename);
/*
 */
cmFileError cm_file_open_mode (cmFile *file, cmFileMode mode, char const *filename);
/*
 */
cmFileError cm_file_new (cmFile *file, cmFileDescriptor fd, cmFileOperations ops, char const *filename);
/*
 */
b32 cm_file_read_at_check (cmFile *file, void *buffer, isize size, i64 offset, isize *bytes_read);
/*
 */
b32 cm_file_write_at_check(cmFile *file, void const *buffer, isize size, i64 offset, isize *bytes_written);
/*
 */
b32 cm_file_read_at (cmFile *file, void *buffer, isize size, i64 offset);
/*
 */
b32 cm_file_write_at (cmFile *file, void const *buffer, isize size, i64 offset);
/*
 */
i64 cm_file_seek (cmFile *file, i64 offset);
/*
 */
i64 cm_file_seek_to_end (cmFile *file);
/*
 */
i64 cm_file_skip (cmFile *file, i64 bytes);
/*
 */
i64 cm_file_tell (cmFile *file);
/*
 */
cmFileError cm_file_close (cmFile *file);
/*
 */
b32 cm_file_read (cmFile *file, void *buffer, isize size);
/*
 */
b32 cm_file_write (cmFile *file, void const *buffer, isize size);
/*
 */
i64 cm_file_size (cmFile *file);
/*
 */
char const *cm_file_name (cmFile *file);
/*
 */
cmFileError cm_file_truncate (cmFile *file, i64 size);
/*
 */
b32 cm_file_has_changed (cmFile *file);
/*
 */
cmFileContents cm_file_read_contents(cmAllocator a, b32 zero_terminate, char const *filepath);
/*
 */
void cm_file_free_contents(cmFileContents *fc);
/*
 */
b32 cm_file_exists (char const *filepath);
/*
 */
cmFileTime cm_file_last_write_time(char const *filepath);
/*
 */
b32 cm_file_copy (char const *existing_filename, char const *new_filename, b32 fail_if_exists);
/*
 */
b32 cm_file_move (char const *existing_filename, char const *new_filename);
/*
 */
b32  cm_file_remove (char const *filename);
/*
 */
b32 cm_path_is_absolute  (char const *path);
/*
 */
b32 cm_path_is_relative  (char const *path);
/*
 */
b32 cm_path_is_root  (char const *path);
/*
 */
char const *cm_path_base_name (char const *path);
/*
 */
char const *cm_path_extension (char const *path);
/*
 */
char * cm_path_get_full_name(cmAllocator a, char const *path);
```
## hash.h
- **Function**
```c
/*
 */
u32 cm_adler32(void const *data, isize len);
/*
 */
u32 cm_crc32(void const *data, isize len);
/*
 */
u64 cm_crc64(void const *data, isize len);
/*
 */
u32 cm_fnv32 (void const *data, isize len);
/*
 */
u64 cm_fnv64 (void const *data, isize len);
/*
 */
u32 cm_fnv32a(void const *data, isize len);
/*
 */
u64 cm_fnv64a(void const *data, isize len);
/*
 */
u32 cm_murmur32(void const *data, isize len);
/*
 */
u64 cm_murmur64(void const *data, isize len);
/*
 */
u32 cm_murmur32_seed(void const *data, isize len, u32 seed);
/*
 */
u64 cm_murmur64_seed(void const *data, isize len, u64 seed);
```
## hashtable.h
```c
#define CM_TABLE(PREFIX, NAME, FUNC, VALUE)
#define CM_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE)
#define  CM_TABLE_DEFINE(NAME, FUNC, VALUE)
```
- **Struct**
```c
/*
 * cmHashTableFindResult
 */
typedef struct cmHashTableFindResult {
  isize hash_index;
  isize entry_prev;
  isize entry_index;
} cmHashTableFindResult;
```
## header.h
```c
#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#define WIN32_LEAN_AND_MEAN 
#define WIN32_MEAN_AND_LEAN 
#define VC_EXTRALEAN
#define NOMINMAX 
```
## memory.h
```c
#define CM_DEFAULT_MEMORY_ALIGNMENT
#define CM_DEFAULT_ALLOCATOR_FLAGS
```
- **Struct**
```c
/*
 * cmVirtualMemory
 */
typedef struct cmVirtualMemory {
  void *data;
  isize size;
} cmVirtualMemory;
/*
 * cmVirtualMemory
 */
typedef enum cmAllocationType {
  cmAllocation_Alloc,
  cmAllocation_Free,
  cmAllocation_FreeAll,
  cmAllocation_Resize,
} cmAllocationType;
/*
 * cmAllocator
 */
typedef struct cmAllocator {
  cmAllocatorProc *proc;
  void *           data;
} cmAllocator;
/*
 * cmAllocatorFlag
 */
typedef enum cmAllocatorFlag {
  cmAllocatorFlag_ClearToZero ,
} cmAllocatorFlag;
/*
 * cmArena
 */
typedef struct cmArena {
  cmAllocator backing;
  void *      physical_start;
  isize       total_size;
  isize       total_allocated;
  isize       temp_count;
} cmArena;
```
- **Function**
```c
/*
 */
b32 cm_is_power_of_two (isize x);
/*
 */
void *cm_align_forward (void *ptr, isize alignment);
/*
 */
void *cm_pointer_add (void *ptr, isize bytes);
/*
 */
void *cm_pointer_sub (void *ptr, isize bytes);
/*
 */
void const *cm_pointer_add_const (void const *ptr, isize bytes);
/*
 */
void const *cm_pointer_sub_const (void const *ptr, isize bytes);
/*
 */
isize cm_pointer_diff (void const *begin, void const *end);
/*
 */
void cm_zero_size (void *ptr, isize size);
/*
 */
void cm_zero_item (void *t)
/*
 */
void cm_zero_array (void *a, count)
/*
 */
void *cm_memcopy (void *dest, void const *source, isize size);
/*
 */
void *cm_memmove (void *dest, void const *source, isize size);
/*
 */
void *cm_memset (void *data, u8 byte_value, isize size);
/*
 */
i32 cm_memcompare (void const *s1, void const *s2, isize size);
/*
 */
void cm_memswap (void *i, void *j, isize size);
/*
 */
void const *cm_memchr (void const *data, u8 byte_value, isize size);
/*
 */
void const *cm_memrchr (void const *data, u8 byte_value, isize size);
/*
 */
cm_memcopy_array (dst, src, count);
/*
 */
cm_memmove_array (dst, src, count) 
/*
 */
cmVirtualMemory cm_virtual_memory (void *data, isize size);
/*
 */
cmVirtualMemory cm_vm_alloc (void *addr, isize size);
/*
 */
b32 cm_vm_free (cmVirtualMemory vm);
/*
 */
cmVirtualMemory cm_vm_trim (cmVirtualMemory vm, isize lead_size, isize size);
/*
 */
b32 cm_vm_purge (cmVirtualMemory vm);
/*
 */
isize cm_virtual_memory_page_size (isize *alignment_out);
/*
 */
void *cm_alloc_align (cmAllocator a, isize size, isize alignment);
/*
 */
void *cm_alloc (cmAllocator a, isize size);
/*
 */
void cm_free (cmAllocator a, void *ptr);
/*
 */
void cm_free_all (cmAllocator a);
/*
 */
void *cm_resize (cmAllocator a, void *ptr, isize old_size, isize new_size);
/*
 */
void *cm_resize_align (cmAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
/*
 */
void *cm_alloc_copy (cmAllocator a, void const *src, isize size);
/*
 */
void *cm_alloc_copy_align (cmAllocator a, void const *src, isize size, isize alignment);
/*
 */
char *cm_alloc_str (cmAllocator a, char const *str);
/*
 */
char *cm_alloc_str_len (cmAllocator a, char const *str, isize len);
/*
 */
cm_alloc_item (allocator_, Type)
/*
 */
cm_alloc_array (allocator_, Type, count)
/*
 */
void *gb_default_resize_align (cmAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
/*
 */
cmAllocator cm_heap_allocator (void);
/*
 */
cm_malloc(sz);
/*
 */
cm_mfree(ptr);
/*
 */
void cm_arena_init_from_memory (cmArena *arena, void *start, isize size);
/*
 */
void cm_arena_init_from_allocator (cmArena *arena, cmAllocator backing, isize size);
/*
 */
void cm_arena_init_sub (cmArena *arena, cmArena *parent_arena, isize size);
/*
 */
void cm_arena_free (cmArena *arena);
/*
 */
isize cm_arena_alignment_of (cmArena *arena, isize alignment);
/*
 */
isize cm_arena_size_remaining (cmArena *arena, isize alignment);
/*
 */
void cm_arena_check (cmArena *arena);
/*
 */
cmAllocator cm_arena_allocator (cmArena *arena);
```
## misc.h
- **Function**
```c
/*
 */
void cm_exit (u32 code);
/*
 */
void cm_yield (void);
/*
 */
void cm_set_env (char const *name, char const *value);
/*
 */
void cm_unset_env (char const *name);
/*
 */
u16 cm_endian_swap16 (u16 i);
/*
 */
u32 cm_endian_swap32 (u32 i);
/*
 */
u64 cm_endian_swap64 (u64 i);
/*
 */
isize cm_count_set_bits (u64 mask);
```
## mutex.h
- **Struct**
```c
/*
 * Windows
 */
typedef struct cmMutex {
  CRITICAL_SECTION win32_critical_section;
} cmMutex;
/*
 * Unix & Linux
 */
typedef struct cmMutex {
  pthread_mutex_t pthread_mutex;
  pthread_mutexattr_t pthread_mutexattr;
} cmMutex;
```
- **Function**
```c
/*
 */
void cm_mutex_init (cmMutex *m);
/*
 */
void cm_mutex_destroy (cmMutex *m);
/*
 */
void cm_mutex_lock (cmMutex *m);
/*
 */
b32  cm_mutex_try_lock (cmMutex *m);
/*
 */
void cm_mutex_unlock (cmMutex *m);
```
## print.h
- **Function**
```c
/*
 */
isize cm_printf (char const *fmt, ...) CM_PRINTF_ARGS(1);
/*
 */
isize cm_printf_va (char const *fmt, va_list va);
/*
 */
isize cm_printf_err (char const *fmt, ...) CM_PRINTF_ARGS(1);
/*
 */
isize cm_printf_err_va (char const *fmt, va_list va);
/*
 */
isize cm_fprintf (cmFile *f, char const *fmt, ...) CM_PRINTF_ARGS(2);
/*
 */
isize cm_fprintf_va (cmFile *f, char const *fmt, va_list va);
/*
 */
char *cm_bprintf (char const *fmt, ...) CM_PRINTF_ARGS(1);
/*
 */
char *cm_bprintf_va (char const *fmt, va_list va);
/*
 */
isize cm_snprintf (char *str, isize n, char const *fmt, ...) CM_PRINTF_ARGS(3);
```
## random.h
- **Struct**
```c
/*
 */
typedef struct cmRandom {
  u32 offsets[8];
  u32 value;
} cmRandom;
```
- **Function**
```c
/*
 */
void cm_random_init (cmRandom *r);
/*
 */
u32 cm_random_gen_u32 (cmRandom *r);
/*
 */
u32 cm_random_gen_u32_unique (cmRandom *r);
/*
 */
u64 cm_random_gen_u64 (cmRandom *r);
/*
 */
isize cm_random_gen_isize (cmRandom *r);
/*
 */
i64 cm_random_range_i64 (cmRandom *r, i64 lower_inc, i64 higher_inc);
/*
 */
isize cm_random_range_isize (cmRandom *r, isize lower_inc, isize higher_inc);
/*
 */
f64 cm_random_range_f64 (cmRandom *r, f64 lower_inc, f64 higher_inc);
```
## semaphore.h
- **Struct**
```c
/*
 * Window
 */
typedef struct cmSemaphore { void *win32_handle;} cmSemaphore;
/*
 * OSX
 */
typedef struct cmSemaphore { semaphore_t osx_handle; } cmSemaphore;
/*
 * Unix
 */
typedef struct cmSemaphore { sem_t unix_handle; }  cmSemaphore;
```
- **Function**
```c
/*
 */
void cm_semaphore_init (cmSemaphore *s);
/*
 */
void cm_semaphore_destroy (cmSemaphore *s);
/*
 */
void cm_semaphore_post (cmSemaphore *s, i32 count);
/*
 */
void cm_semaphore_release (cmSemaphore *s);
/*
 */
void cm_semaphore_wait (cmSemaphore *s);
```
## sortsearch.h
- **Function**
```c
/*
 */
int (*cm_i16_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_i32_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_i64_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_isize_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_str_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_f32_cmp (isize offset))(void const *a, void const *b);
/*
 */
int (*cm_f64_cmp (isize offset))(void const *a, void const *b);
/*
 */
cm_sort_array(array, count, compare_proc) ;
/*
 */
void cm_sort(void *base, isize count, isize size, cmCompareProc compare_proc);
/*
 */
cm_radix_sort(Type);
/*
 */
void cm_radix_sort_u8 (u8 *items, u8 *temp, isize count);
/*
 */
void cm_radix_sort_u16 (u16 *items, u16 *temp, isize count);
/*
 */
void cm_radix_sort_u32 (u32 *items, u32 *temp, isize count);
/*
 */
void cm_radix_sort_u64 (u64 *items, u64 *temp, isize count);
/*
 */
cm_binary_search_array(array, count, key, compare_proc);
/*
 */
isize cm_binary_search(void const *base, isize count, isize size, void const *key, cmCompareProc compare_proc);
/*
 */
cm_shuffle_array(array, count);
/*
 */
void cm_shuffle(void *base, isize count, isize size);
/*
 */
cm_reverse_array(array, count);
/*
 */
void cm_reverse(void *base, isize count, isize size);
```
## string.h
- **Struct**
```c
/*
 */
typedef char *cmString;
/*
 */
typedef struct cmStringHeader {
  cmAllocator allocator;
  isize       length;
  isize       capacity;
} cmStringHeader;
```
- **Function**
```c
/*
 */
cmString cm_string_make_reserve (cmAllocator a, isize capacity);
/*
 */
cmString cm_string_make (cmAllocator a, char const *str);
/*
 */
cmString cm_string_make_length (cmAllocator a, void const *str, isize num_bytes);
/*
 */
void  cm_string_free (cmString str);
/*
 */
cmString cm_string_duplicate (cmAllocator a, cmString const str);
/*
 */
isize cm_string_length (cmString const str);
/*
 */
isize cm_string_capacity (cmString const str);
/*
 */
isize  cm_string_available_space(cmString const str);
/*
 */
void  cm_string_clear (cmString str);
/*
 */
cmString cm_string_append (cmString str, cmString const other);
/*
 */
cmString cm_string_append_length  (cmString str, void const *other, isize num_bytes);
/*
 */
cmString cm_string_appendc (cmString str, char const *other);
/*
 */
cmString cm_string_append_rune (cmString str, Rune r);
/*
 */
cmString cm_string_append_fmt  (cmString str, char const *fmt, ...);
/*
 */
cmString cm_string_set (cmString str, char const *cstr);
/*
 */
cmString cm_string_make_space_for (cmString str, isize add_len);
/*
 */
isize  cm_string_allocation_size(cmString const str);
/*
 */
b32 cm_string_are_equal (cmString const lhs, cmString const rhs);
/*
 */
cmString cm_string_trim (cmString str, char const *cut_set);
/*
 */
cmString cm_string_trim_space (cmString str);
```
## thread.h
- **Struct**
```c
/*
 * Window
 */
typedef struct cmThread {
  void *        win32_handle;
  cmThreadProc * proc;
  void *         user_data;
  isize          user_index;
  isize volatile return_value;
  cmSemaphore   semaphore;
  isize         stack_size;
  b32 volatile  is_running;
} cmThread;
/*
 * Posix
 */
typedef struct cmThread {
  pthread_t     posix_handle;
  cmThreadProc * proc;
  void *         user_data;
  isize          user_index;
  isize volatile return_value;
  cmSemaphore   semaphore;
  isize         stack_size;
  b32 volatile  is_running;
} cmThread;
/*
 */
typedef struct cmSync {
  i32 target;  // Target Number of threads
  i32 current; // Threads to hit
  i32 waiting; // Threads waiting
  cmMutex start;
  cmMutex mutex;
  cmSemaphore release;
} cmSync;
```
- **Function**
```c
/*
 */
void cm_thread_init (cmThread *t);
/*
 */
void cm_thread_destroy (cmThread *t);
/*
 */
void cm_thread_start (cmThread *t, cmThreadProc *proc, void *data);
/*
 */
void cm_thread_start_with_stack (cmThread *t, cmThreadProc *proc, void *data, isize stack_size);
/*
 */
void cm_thread_join (cmThread *t);
/*
 */
b32  cm_thread_is_running (cmThread const *t);
/*
 */
u32  cm_thread_current_id (void);
/*
 */
void cm_thread_set_name (cmThread *t, char const *name);
/*
 */
void cm_sync_init (cmSync *s);
/*
 */
void cm_sync_destroy (cmSync *s);
/*
 */
void cm_sync_set_target (cmSync *s, i32 count);
/*
 */
void cm_sync_release (cmSync *s);
/*
 */
i32  cm_sync_reach (cmSync *s);
/*
 */
void cm_sync_reach_and_wait (cmSync *s);
```
## time.h
- **Function**
```c
/*
 */
u64 cm_rdtsc (void);
/*
 */
f64 cm_time_now (void);
/*
 */
u64 cm_utc_time_now(void);
/*
 */
void cm_sleep_ms (u32 ms);
```
## utf8.h
- **Function**
```c
/*
 */
isize cm_utf8_strlen (u8 const *str);
/*
 */
isize cm_utf8_strnlen(u8 const *str, isize max_len);
/*
 */
u16 *cm_utf8_to_ucs2 (u16 *buffer, isize len, u8 const *str);
/*
 */
u8 * cm_ucs2_to_utf8 (u8 *buffer, isize len, u16 const *str);
/*
 */
u16 *cm_utf8_to_ucs2_buf (u8 const *str);
/*
 */
u8 * cm_ucs2_to_utf8_buf (u16 const *str);
/*
 */
isize cm_utf8_decode (u8 const *str, isize str_len, Rune *codepoint);
/*
 */
isize cm_utf8_codepoint_size (u8 const *str, isize str_len);
/*
 */
isize cm_utf8_encode_rune (u8 buf[4], Rune r);
```
## utils.h
```c
#define NULL
#define inline
#define cm_restrict
#define cm_inline
#define cm_no_inline
#define cm_thread_local
#define cast(Type)
#define cm_size_of(x)
#define cm_count_of(x)
#define cm_offset_of(Type, element)
#define cm_align_of(Type)
#define cm_swap(Type, a, b)
#define cm_global
#define cm_internal
#define cm_local_persist
#define cm_unused(x)
#define vprtcast(x)
#define voidcast(x) // (void *)
#define CM_BIT(x)
#define CM_MIN(a, b)
#define CM_MAX(a, b)
#define CM_MIN3(a, b, c)
#define CM_MAX3(a, b, c)
#define CM_CLAMP(x, lower, upper)
#define CM_CLAMP01(x)
#define CM_IS_BETWEEN(x, lower, upper)
#define CM_ABS(x)
#define CM_MASK_SET(var, set, mask)
#define CM_PRINTF_ARGS(FMT)
```