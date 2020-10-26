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

#ifndef CM_FILE_H
#define CM_FILE_H

#include "memory.h"

CM_BEGIN_EXTERN

//
// File Handling
//

typedef u32 cmFileMode;
typedef enum cmFileModeFlag {
	cmFileMode_Read       = CM_BIT(0),
	cmFileMode_Write      = CM_BIT(1),
	cmFileMode_Append     = CM_BIT(2),
	cmFileMode_Rw         = CM_BIT(3),

    cmFileMode_Modes = cmFileMode_Read | cmFileMode_Write | cmFileMode_Append | cmFileMode_Rw,
} cmFileModeFlag;

// NOTE(bill): Only used internally and for the file operations
typedef enum cmSeekWhenceType {
	cmSeekWhence_Begin   = 0,
	cmSeekWhence_Current = 1,
	cmSeekWhence_End     = 2,
} cmSeekWhenceType;

typedef enum cmFileError {
	cmFileError_None,
	cmFileError_Invalid,
	cmFileError_InvalidFilename,
	cmFileError_Exists,
	cmFileError_NotExists,
	cmFileError_Permission,
	cmFileError_TruncationFailure,
} cmFileError;

typedef union cmFileDescriptor {
	void *  p;
	intptr  i;
	uintptr u;
} cmFileDescriptor;

typedef struct cmFileOperations cmFileOperations;

#define CM_FILE_OPEN_PROC(name)     cmFileError name(cmFileDescriptor *fd, cmFileOperations *ops, cmFileMode mode, char const *filename)
#define CM_FILE_READ_AT_PROC(name)  b32         name(cmFileDescriptor fd, void *buffer, isize size, i64 offset, isize *bytes_read)
#define CM_FILE_WRITE_AT_PROC(name) b32         name(cmFileDescriptor fd, void const *buffer, isize size, i64 offset, isize *bytes_written)
#define CM_FILE_SEEK_PROC(name)     b32         name(cmFileDescriptor fd, i64 offset, cmSeekWhenceType whence, i64 *new_offset)
#define CM_FILE_CLOSE_PROC(name)    void        name(cmFileDescriptor fd)
typedef CM_FILE_OPEN_PROC(cmFileOpenProc);
typedef CM_FILE_READ_AT_PROC(cmFileReadProc);
typedef CM_FILE_WRITE_AT_PROC(cmFileWriteProc);
typedef CM_FILE_SEEK_PROC(cmFileSeekProc);
typedef CM_FILE_CLOSE_PROC(cmFileCloseProc);

struct cmFileOperations {
	cmFileReadProc  *read_at;
	cmFileWriteProc *write_at;
	cmFileSeekProc  *seek;
	cmFileCloseProc *close;
};

extern cmFileOperations const cmDefaultFileOperations;


// typedef struct gbDirInfo {
// 	u8 *buf;
// 	isize buf_count;
// 	isize buf_pos;
// } gbDirInfo;

typedef u64 cmFileTime;

typedef struct cmFile {
	cmFileOperations ops;
	cmFileDescriptor fd;
	char const *     filename;
	cmFileTime       last_write_time;
	// gbDirInfo *   dir_info; // TODO(bill): Get directory info
} cmFile;

// TODO(bill): gbAsyncFile

typedef enum cmFileStandardType {
	cmFileStandard_Input,
	cmFileStandard_Output,
	cmFileStandard_Error,

	cmFileStandard_Count,
} cmFileStandardType;

CM_DEF cmFile *const cm_file_get_standard(cmFileStandardType std);

CM_DEF cmFileError cm_file_create        (cmFile *file, char const *filename);
CM_DEF cmFileError cm_file_open          (cmFile *file, char const *filename);
CM_DEF cmFileError cm_file_open_mode     (cmFile *file, cmFileMode mode, char const *filename);
CM_DEF cmFileError cm_file_new           (cmFile *file, cmFileDescriptor fd, cmFileOperations ops, char const *filename);
CM_DEF b32         cm_file_read_at_check (cmFile *file, void *buffer, isize size, i64 offset, isize *bytes_read);
CM_DEF b32         cm_file_write_at_check(cmFile *file, void const *buffer, isize size, i64 offset, isize *bytes_written);
CM_DEF b32         cm_file_read_at       (cmFile *file, void *buffer, isize size, i64 offset);
CM_DEF b32         cm_file_write_at      (cmFile *file, void const *buffer, isize size, i64 offset);
CM_DEF i64         cm_file_seek          (cmFile *file, i64 offset);
CM_DEF i64         cm_file_seek_to_end   (cmFile *file);
CM_DEF i64         cm_file_skip          (cmFile *file, i64 bytes); // NOTE(bill): Skips a certain amount of bytes
CM_DEF i64         cm_file_tell          (cmFile *file);
CM_DEF cmFileError cm_file_close         (cmFile *file);
CM_DEF b32         cm_file_read          (cmFile *file, void *buffer, isize size);
CM_DEF b32         cm_file_write         (cmFile *file, void const *buffer, isize size);
CM_DEF i64         cm_file_size          (cmFile *file);
CM_DEF char const *cm_file_name          (cmFile *file);
CM_DEF cmFileError cm_file_truncate      (cmFile *file, i64 size);
CM_DEF b32         cm_file_has_changed   (cmFile *file); // NOTE(bill): Changed since lasted checked
// TODO(bill):
// gbFileError gb_file_temp(gbFile *file);
//

typedef struct cmFileContents {
	cmAllocator allocator;
	void *      data;
	isize       size;
} cmFileContents;


CM_DEF cmFileContents cm_file_read_contents(cmAllocator a, b32 zero_terminate, char const *filepath);
CM_DEF void           cm_file_free_contents(cmFileContents *fc);


// TODO(bill): Should these have different na,es as they do not take in a gbFile * ???
CM_DEF b32        cm_file_exists         (char const *filepath);
CM_DEF cmFileTime cm_file_last_write_time(char const *filepath);
CM_DEF b32        cm_file_copy           (char const *existing_filename, char const *new_filename, b32 fail_if_exists);
CM_DEF b32        cm_file_move           (char const *existing_filename, char const *new_filename);
CM_DEF b32        cm_file_remove         (char const *filename);


#ifndef CM_PATH_SEPARATOR
	#if defined(CM_SYS_WINDOWS)
		#define CM_PATH_SEPARATOR '\\'
	#else
		#define CM_PATH_SEPARATOR '/'
	#endif
#endif

CM_DEF b32         cm_path_is_absolute  (char const *path);
CM_DEF b32         cm_path_is_relative  (char const *path);
CM_DEF b32         cm_path_is_root      (char const *path);
CM_DEF char const *cm_path_base_name    (char const *path);
CM_DEF char const *cm_path_extension    (char const *path);
CM_DEF char *      cm_path_get_full_name(cmAllocator a, char const *path);

CM_END_EXTERN

#endif //CM_FILE_H