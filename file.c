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

#include "file.h"
#include "utils.h"
#include "header.h"
#include "char.h"

#include "debug.h"

CM_BEGIN_EXTERN

#if defined(CM_SYS_WINDOWS)

	cm_internal wchar_t *
    cm__alloc_utf8_to_ucs2(cmAllocator a, char const *text, isize *w_len_) {
		wchar_t *w_text = NULL;
		isize len = 0, w_len = 0, w_len1 = 0;
		if (text == NULL) {
			if (w_len_) *w_len_ = w_len;
			return NULL;
		}
		len = cm_strlen(text);
		if (len == 0) {
			if (w_len_) *w_len_ = w_len;
			return NULL;
		}
		w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, cast(int)len, NULL, 0);
		if (w_len == 0) {
			if (w_len_) *w_len_ = w_len;
			return NULL;
		}
		w_text = cm_alloc_array(a, wchar_t, w_len+1);
		w_len1 = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, cast(int)len, w_text, cast(int)w_len);
		if (w_len1 == 0) {
			cm_free(a, w_text);
			if (w_len_) *w_len_ = 0;
			return NULL;
		}
		w_text[w_len] = 0;
		if (w_len_) *w_len_ = w_len;
		return w_text;
	}

	cm_internal 
    CM_FILE_SEEK_PROC(cm__win32_file_seek) {
		LARGE_INTEGER li_offset;
		li_offset.QuadPart = offset;
		if (!SetFilePointerEx(fd.p, li_offset, &li_offset, whence)) {
			return false;
		}

		if (new_offset) *new_offset = li_offset.QuadPart;
		return true;
	}

	cm_internal 
    CM_FILE_READ_AT_PROC(cm__win32_file_read) {
		b32 result = false;
		DWORD size_ = cast(DWORD)(size > I32_MAX ? I32_MAX : size);
		DWORD bytes_read_;
		cm__win32_file_seek(fd, offset, cmSeekWhence_Begin, NULL);
		if (ReadFile(fd.p, buffer, size_, &bytes_read_, NULL)) {
			if (bytes_read) *bytes_read = bytes_read_;
			result = true;
		}

		return result;
	}

	cm_internal 
    CM_FILE_WRITE_AT_PROC(cm__win32_file_write) {
		DWORD size_ = cast(DWORD)(size > I32_MAX ? I32_MAX : size);
		DWORD bytes_written_;
		cm__win32_file_seek(fd, offset, cmSeekWhence_Begin, NULL);
		if (WriteFile(fd.p, buffer, size_, &bytes_written_, NULL)) {
			if (bytes_written) *bytes_written = bytes_written_;
			return true;
		}
		return false;
	}

	cm_internal 
    CM_FILE_CLOSE_PROC(cm__win32_file_close) {
		CloseHandle(fd.p);
	}

	cmFileOperations const cmDefaultFileOperations = {
		cm__win32_file_read,
		cm__win32_file_write,
		cm__win32_file_seek,
		cm__win32_file_close
	};

	cm_no_inline 
    CM_FILE_OPEN_PROC(cm__win32_file_open) {
		DWORD desired_access;
		DWORD creation_disposition;
		void *handle;
		wchar_t *w_text;

		switch (mode & cmFileMode_Modes) {
		case cmFileMode_Read:
			desired_access = GENERIC_READ;
			creation_disposition = OPEN_EXISTING;
			break;
		case cmFileMode_Write:
			desired_access = GENERIC_WRITE;
			creation_disposition = CREATE_ALWAYS;
			break;
		case cmFileMode_Append:
			desired_access = GENERIC_WRITE;
			creation_disposition = OPEN_ALWAYS;
			break;
		case cmFileMode_Read | cmFileMode_Rw:
			desired_access = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = OPEN_EXISTING;
			break;
		case cmFileMode_Write | cmFileMode_Rw:
			desired_access = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = CREATE_ALWAYS;
			break;
		case cmFileMode_Append | cmFileMode_Rw:
			desired_access = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = OPEN_ALWAYS;
			break;
		default:
			CM_PANIC("Invalid file mode");
			return cmFileError_Invalid;
		}

		w_text = cm__alloc_utf8_to_ucs2(cm_heap_allocator(), filename, NULL);
		if (w_text == NULL) {
			return cmFileError_InvalidFilename;
		}
		handle = CreateFileW(w_text,
		                     desired_access,
		                     FILE_SHARE_READ|FILE_SHARE_DELETE, NULL,
		                     creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL);

		cm_free(cm_heap_allocator(), w_text);

		if (handle == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			switch (err) {
			case ERROR_FILE_NOT_FOUND: return cmFileError_NotExists;
			case ERROR_FILE_EXISTS:    return cmFileError_Exists;
			case ERROR_ALREADY_EXISTS: return cmFileError_Exists;
			case ERROR_ACCESS_DENIED:  return cmFileError_Permission;
			}
			return cmFileError_Invalid;
		}

		if (mode & cmFileMode_Append) {
			LARGE_INTEGER offset = {0};
			if (!SetFilePointerEx(handle, offset, NULL, cmSeekWhence_End)) {
				CloseHandle(handle);
				return cmFileError_Invalid;
			}
		}

		fd->p = handle;
		*ops = cmDefaultFileOperations;
		return cmFileError_None;
	}

#else // POSIX
	cm_internal 
    CM_FILE_SEEK_PROC(cm__posix_file_seek) {
		#if defined(CM_SYS_OSX)
		i64 res = lseek(fd.i, offset, whence);
		#else
		i64 res = lseek64(fd.i, offset, whence);
		#endif
		if (res < 0) return false;
		if (new_offset) *new_offset = res;
		return true;
	}

	cm_internal 
    CM_FILE_READ_AT_PROC(cm__posix_file_read) {
		isize res = pread(fd.i, buffer, size, offset);
		if (res < 0) return false;
		if (bytes_read) *bytes_read = res;
		return true;
	}

	cm_internal 
    CM_FILE_WRITE_AT_PROC(cm__posix_file_write) {
		isize res;
		i64 curr_offset = 0;
		cm__posix_file_seek(fd, 0, gbSeekWhence_Current, &curr_offset);
		if (curr_offset == offset) {
			// NOTE(bill): Writing to stdout et al. doesn't like pwrite for numerous reasons
			res = write(cast(int)fd.i, buffer, size);
		} else {
			res = pwrite(cast(int)fd.i, buffer, size, offset);
		}
		if (res < 0) return false;
		if (bytes_written) *bytes_written = res;
		return true;
	}

	cm_internal 
    CM_FILE_CLOSE_PROC(cm__posix_file_close) {
		close(fd.i);
	}

	cmFileOperations const cmDefaultFileOperations = {
		cm__posix_file_read,
		cm__posix_file_write,
		cm__posix_file_seek,
		cm__posix_file_close
	};

	cm_no_inline 
    CM_FILE_OPEN_PROC(cm__posix_file_open) {
		i32 os_mode;
		switch (mode & gbFileMode_Modes) {
		case cmFileMode_Read:
			os_mode = O_RDONLY;
			break;
		case cmFileMode_Write:
			os_mode = O_WRONLY | O_CREAT | O_TRUNC;
			break;
		case cmFileMode_Append:
			os_mode = O_WRONLY | O_APPEND | O_CREAT;
			break;
		case cmFileMode_Read | cmFileMode_Rw:
			os_mode = O_RDWR;
			break;
		case cmFileMode_Write | cmFileMode_Rw:
			os_mode = O_RDWR | O_CREAT | O_TRUNC;
			break;
		case cmFileMode_Append | cmFileMode_Rw:
			os_mode = O_RDWR | O_APPEND | O_CREAT;
			break;
		default:
			CM_PANIC("Invalid file mode");
			return cmFileError_Invalid;
		}

		fd->i = open(filename, os_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (fd->i < 0) {
			// TODO(bill): More file errors
			return cmFileError_Invalid;
		}

		*ops = cmDefaultFileOperations;
		return cmFileError_None;
	}

#endif

cmFileError 
cm_file_new(cmFile *f, cmFileDescriptor fd, cmFileOperations ops, char const *filename) {
	cmFileError err = cmFileError_None;
	isize len = cm_strlen(filename);

	// cm_printf_err("cm_file_new: %s\n", filename);

	f->ops = ops;
	f->fd = fd;
	f->filename = cm_alloc_array(cm_heap_allocator(), char, len+1);
	cm_memcopy(cast(char *)f->filename, cast(char *)filename, len+1);
	f->last_write_time = cm_file_last_write_time(f->filename);

	return err;
}


cmFileError cm_file_open_mode(cmFile *f, cmFileMode mode, char const *filename) {
	cmFileError err;
#if defined(CM_SYS_WINDOWS)
	err = cm__win32_file_open(&f->fd, &f->ops, mode, filename);
#else
	err = cm__posix_file_open(&f->fd, &f->ops, mode, filename);
#endif
	if (err == cmFileError_None) {
		return cm_file_new(f, f->fd, f->ops, filename);
	}
	return err;
}


cmFileError 
cm_file_close(cmFile *f) {
	if (f == NULL) {
		return cmFileError_Invalid;
	}

#if defined(CM_COMPILER_MSVC)
	if (f->filename != NULL) {
		cm_free(cm_heap_allocator(), cast(char *)f->filename);
	}
#else
	// TODO HACK(bill): Memory Leak!!!
#endif

#if defined(CM_SYS_WINDOWS)
	if (f->fd.p == INVALID_HANDLE_VALUE) {
		return cmFileError_Invalid;
	}
#else
	if (f->fd.i < 0) {
		return cmFileError_Invalid;
	}
#endif

	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	f->ops.close(f->fd);

	return cmFileError_None;
}


cm_inline b32 
cm_file_read_at_check(cmFile *f, void *buffer, isize size, i64 offset, isize *bytes_read) {
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	return f->ops.read_at(f->fd, buffer, size, offset, bytes_read);
}

cm_inline b32 
cm_file_write_at_check(cmFile *f, void const *buffer, isize size, i64 offset, isize *bytes_written) {
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	return f->ops.write_at(f->fd, buffer, size, offset, bytes_written);
}

cm_inline b32 
cm_file_read_at(cmFile *f, void *buffer, isize size, i64 offset) {
	return cm_file_read_at_check(f, buffer, size, offset, NULL);
}

cm_inline b32 
cm_file_write_at(cmFile *f, void const *buffer, isize size, i64 offset) {
	return cm_file_write_at_check(f, buffer, size, offset, NULL);
}

cm_inline i64 
cm_file_seek(cmFile *f, i64 offset) {
	i64 new_offset = 0;
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	f->ops.seek(f->fd, offset, cmSeekWhence_Begin, &new_offset);
	return new_offset;
}


cm_inline i64 
cm_file_seek_to_end(cmFile *f) {
	i64 new_offset = 0;
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	f->ops.seek(f->fd, 0, cmSeekWhence_End, &new_offset);
	return new_offset;
}

// NOTE(bill): Skips a certain amount of bytes
cm_inline i64 
cm_file_skip(cmFile *f, i64 bytes) {
	i64 new_offset = 0;
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	f->ops.seek(f->fd, bytes, cmSeekWhence_Current, &new_offset);
	return new_offset;
}

cm_inline i64 
cm_file_tell(cmFile *f) {
	i64 new_offset = 0;
	if (!f->ops.read_at) f->ops = cmDefaultFileOperations;
	f->ops.seek(f->fd, 0, cmSeekWhence_Current, &new_offset);
	return new_offset;
}

cm_inline b32 
cm_file_read (cmFile *f, void *buffer, isize size) { 
    return cm_file_read_at(f, buffer, size, cm_file_tell(f)); 
}

cm_inline b32 
cm_file_write(cmFile *f, void const *buffer, isize size) { 
    return cm_file_write_at(f, buffer, size, cm_file_tell(f)); 
}

cmFileError 
cm_file_create(cmFile *f, char const *filename) {
	return cm_file_open_mode(f, cmFileMode_Write|cmFileMode_Rw, filename);
}

cmFileError 
cm_file_open(cmFile *f, char const *filename) {
	return cm_file_open_mode(f, cmFileMode_Read, filename);
}

char const *
cm_file_name(cmFile *f) { 
    return f->filename ? f->filename : ""; 
}

cm_inline b32 
cm_file_has_changed(cmFile *f) {
	b32 result = false;
	cmFileTime last_write_time = cm_file_last_write_time(f->filename);
	if (f->last_write_time != last_write_time) {
		result = true;
		f->last_write_time = last_write_time;
	}
	return result;
}

// TODO(bill): Is this a bad idea?
cm_global b32    
cm__std_file_set = false;

cm_global cmFile 
cm__std_files[cmFileStandard_Count] = {{0}};


#if defined(CM_SYS_WINDOWS)

cm_inline cmFile *const 
cm_file_get_standard(cmFileStandardType std) {
	if (!cm__std_file_set) {
	#define CM__SET_STD_FILE(type, v) cm__std_files[type].fd.p = v; cm__std_files[type].ops = cmDefaultFileOperations
		CM__SET_STD_FILE(cmFileStandard_Input,  GetStdHandle(STD_INPUT_HANDLE));
		CM__SET_STD_FILE(cmFileStandard_Output, GetStdHandle(STD_OUTPUT_HANDLE));
		CM__SET_STD_FILE(cmFileStandard_Error,  GetStdHandle(STD_ERROR_HANDLE));
	#undef CM__SET_STD_FILE
		cm__std_file_set = true;
	}
	return &cm__std_files[std];
}

cm_inline i64 
cm_file_size(cmFile *f) {
	LARGE_INTEGER size;
	GetFileSizeEx(f->fd.p, &size);
	return size.QuadPart;
}

cmFileError 
cm_file_truncate(cmFile *f, i64 size) {
	cmFileError err = cmFileError_None;
	i64 prev_offset = cm_file_tell(f);
	cm_file_seek(f, size);
	if (!SetEndOfFile(f)) {
		err = cmFileError_TruncationFailure;
	}
	cm_file_seek(f, prev_offset);
	return err;
}

b32 
cm_file_exists(char const *name) {
	WIN32_FIND_DATAW data;
	wchar_t *w_text;
	void *handle;
	b32 found = false;
	cmAllocator a = cm_heap_allocator();

	w_text = cm__alloc_utf8_to_ucs2(a, name, NULL);
	if (w_text == NULL) {
		return false;
	}
	handle = FindFirstFileW(w_text, &data);
	cm_free(a, w_text);
	found = handle != INVALID_HANDLE_VALUE;
	if (found) FindClose(handle);
	return found;
}

#else // POSIX

cm_inline cmFile *const 
cm_file_get_standard(cmFileStandardType std) {
	if (!cm__std_file_set) {
	#define CM__SET_STD_FILE(type, v) cm__std_files[type].fd.i = v; cm__std_files[type].ops = cmDefaultFileOperations
		CM__SET_STD_FILE(cmFileStandard_Input,  0);
		CM__SET_STD_FILE(cmFileStandard_Output, 1);
		CM__SET_STD_FILE(cmFileStandard_Error,  2);
	#undef CM__SET_STD_FILE
		cm__std_file_set = true;
	}
	return &cm__std_files[std];
}

cm_inline i64 
cm_file_size(cmFile *f) {
	i64 size = 0;
	i64 prev_offset = cm_file_tell(f);
	cm_file_seek_to_end(f);
	size = cm_file_tell(f);
	cm_file_seek(f, prev_offset);
	return size;
}

cm_inline cmFileError 
cm_file_truncate(cmFile *f, i64 size) {
	gbFileError err = gbFileError_None;
	int i = ftruncate(f->fd.i, size);
	if (i != 0) err = gbFileError_TruncationFailure;
	return err;
}

cm_inline b32 
cm_file_exists(char const *name) {
	return access(name, F_OK) != -1;
}
#endif


#if defined(CM_SYS_WINDOWS)
cmFileTime 
cm_file_last_write_time(char const *filepath) {
	ULARGE_INTEGER li = {0};
	FILETIME last_write_time = {0};
	WIN32_FILE_ATTRIBUTE_DATA data = {0};
	cmAllocator a = cm_heap_allocator();

	wchar_t *w_text = cm__alloc_utf8_to_ucs2(a, filepath, NULL);
	if (w_text == NULL) {
		return 0;
	}

	if (GetFileAttributesExW(w_text, GetFileExInfoStandard, &data)) {
		last_write_time = data.ftLastWriteTime;
	}
	cm_free(a, w_text);

	li.LowPart = last_write_time.dwLowDateTime;
	li.HighPart = last_write_time.dwHighDateTime;
	return cast(cmFileTime)li.QuadPart;
}

cm_inline b32 
cm_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists) {
	wchar_t *w_old = NULL;
	wchar_t *w_new = NULL;
	cmAllocator a = cm_heap_allocator();
	b32 result = false;

	w_old = cm__alloc_utf8_to_ucs2(a, existing_filename, NULL);
	if (w_old == NULL) {
		return false;
	}
	w_new = cm__alloc_utf8_to_ucs2(a, new_filename, NULL);
	if (w_new != NULL) {
		result = CopyFileW(w_old, w_new, fail_if_exists);
	}
	cm_free(a, w_new);
	cm_free(a, w_old);
	return result;
}

cm_inline b32 
cm_file_move(char const *existing_filename, char const *new_filename) {
	wchar_t *w_old = NULL;
	wchar_t *w_new = NULL;
	cmAllocator a = cm_heap_allocator();
	b32 result = false;

	w_old = cm__alloc_utf8_to_ucs2(a, existing_filename, NULL);
	if (w_old == NULL) {
		return false;
	}
	w_new = cm__alloc_utf8_to_ucs2(a, new_filename, NULL);
	if (w_new != NULL) {
		result = MoveFileW(w_old, w_new);
	}
	cm_free(a, w_new);
	cm_free(a, w_old);
	return result;
}

b32 cm_file_remove(char const *filename) {
	wchar_t *w_filename = NULL;
	cmAllocator a = cm_heap_allocator();
	b32 result = false;
	w_filename = cm__alloc_utf8_to_ucs2(a, filename, NULL);
	if (w_filename == NULL) {
		return false;
	}
	result = DeleteFileW(w_filename);
	cm_free(a, w_filename);
	return result;
}

#else

cmFileTime 
cm_file_last_write_time(char const *filepath) {
	time_t result = 0;
	struct stat file_stat;

	if (stat(filepath, &file_stat) == 0) {
		result = file_stat.st_mtime;
	}

	return cast(gbFileTime)result;
}


cm_inline b32 
cm_file_copy(char const *existing_filename, char const *new_filename, b32 fail_if_exists) {
#if defined(CM_SYS_OSX)
	return copyfile(existing_filename, new_filename, NULL, COPYFILE_DATA) == 0;
#else
	isize size;
	int existing_fd = open(existing_filename, O_RDONLY, 0);
	int new_fd      = open(new_filename, O_WRONLY|O_CREAT, 0666);

	struct stat stat_existing;
	fstat(existing_fd, &stat_existing);

	size = sendfile(new_fd, existing_fd, 0, stat_existing.st_size);

	close(new_fd);
	close(existing_fd);

	return size == stat_existing.st_size;
#endif
}

cm_inline b32 
cm_file_move(char const *existing_filename, char const *new_filename) {
	if (link(existing_filename, new_filename) == 0) {
		return unlink(existing_filename) != -1;
	}
	return false;
}

b32 
cm_file_remove(char const *filename) {
#if defined(CM_SYS_OSX)
	return unlink(filename) != -1;
#else
	return remove(filename) == 0;
#endif
}

#endif

cmFileContents 
cm_file_read_contents(cmAllocator a, b32 zero_terminate, char const *filepath) {
	cmFileContents result = {0};
	cmFile file = {0};

	result.allocator = a;

	if (cm_file_open(&file, filepath) == cmFileError_None) {
		isize file_size = cast(isize)cm_file_size(&file);
		if (file_size > 0) {
			result.data = cm_alloc(a, zero_terminate ? file_size+1 : file_size);
			result.size = file_size;
			cm_file_read_at(&file, result.data, result.size, 0);
			if (zero_terminate) {
				u8 *str = cast(u8 *)result.data;
				str[file_size] = '\0';
			}
		}
		cm_file_close(&file);
	}

	return result;
}

void 
cm_file_free_contents(cmFileContents *fc) {
	CM_ASSERT_NOT_NULL(fc->data);
	cm_free(fc->allocator, fc->data);
	fc->data = NULL;
	fc->size = 0;
}

cm_inline b32 
cm_path_is_absolute(char const *path) {
	b32 result = false;
	CM_ASSERT_NOT_NULL(path);
#if defined(CM_SYS_WINDOWS)
	result == (cm_strlen(path) > 2) &&
	          cm_char_is_alpha(path[0]) &&
	          (path[1] == ':' && path[2] == CM_PATH_SEPARATOR);
#else
	result = (cm_strlen(path) > 0 && path[0] == CM_PATH_SEPARATOR);
#endif
	return result;
}

cm_inline b32 
cm_path_is_relative(char const *path) { 
    return !cm_path_is_absolute(path);
}

cm_inline b32 
cm_path_is_root(char const *path) {
	b32 result = false;
	CM_ASSERT_NOT_NULL(path);
#if defined(CM_SYS_WINDOWS)
	result = cm_path_is_absolute(path) && (cm_strlen(path) == 3);
#else
	result = cm_path_is_absolute(path) && (cm_strlen(path) == 1);
#endif
	return result;
}

cm_inline char const *
cm_path_base_name(char const *path) {
	char const *ls;
	CM_ASSERT_NOT_NULL(path);
	ls = cm_char_last_occurence(path, '/');
	return (ls == NULL) ? path : ls+1;
}

cm_inline char const *
cm_path_extension(char const *path) {
	char const *ld;
	CM_ASSERT_NOT_NULL(path);
	ld = cm_char_last_occurence(path, '.');
	return (ld == NULL) ? NULL : ld+1;
}


#if !defined(_WINDOWS_) && defined(CM_SYS_WINDOWS)
CM_DLL_IMPORT DWORD WINAPI GetFullPathNameA(char const *lpFileName, DWORD nBufferLength, char *lpBuffer, char **lpFilePart);
CM_DLL_IMPORT DWORD WINAPI GetFullPathNameW(wchar_t const *lpFileName, DWORD nBufferLength, wchar_t *lpBuffer, wchar_t **lpFilePart);
#endif

char *
cm_path_get_full_name(cmAllocator a, char const *path) {
#if defined(CM_SYS_WINDOWS)
// TODO(bill): Make UTF-8
	wchar_t *w_path = NULL;
	wchar_t *w_fullpath = NULL;
	isize w_len = 0;
	isize new_len = 0;
	isize new_len1 = 0;
	char *new_path = 0;
	w_path = cm__alloc_utf8_to_ucs2(cm_heap_allocator(), path, NULL);
	if (w_path == NULL) {
		return NULL;
	}
	w_len = GetFullPathNameW(w_path, 0, NULL, NULL);
	if (w_len == 0) {
		return NULL;
	}
	w_fullpath = cm_alloc_array(cm_heap_allocator(), wchar_t, w_len+1);
	GetFullPathNameW(w_path, cast(int)w_len, w_fullpath, NULL);
	w_fullpath[w_len] = 0;
	cm_free(cm_heap_allocator(), w_path);

	new_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w_fullpath, cast(int)w_len, NULL, 0, NULL, NULL);
	if (new_len == 0) {
		cm_free(cm_heap_allocator(), w_fullpath);
		return NULL;
	}
	new_path = cm_alloc_array(a, char, new_len+1);
	new_len1 = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w_fullpath, cast(int)w_len, new_path, cast(int)new_len, NULL, NULL);
	if (new_len1 == 0) {
		cm_free(cm_heap_allocator(), w_fullpath);
		cm_free(a, new_path);
		return NULL;
	}
	new_path[new_len] = 0;
	return new_path;
#else
	char *p, *result, *fullpath = NULL;
	isize len;
	p = realpath(path, NULL);
	fullpath = p;
	if (p == NULL) {
		// NOTE(bill): File does not exist
		fullpath = cast(char *)path;
	}

	len = cm_strlen(fullpath);

	result = cm_alloc_array(a, char, len + 1);
	cm_memmove(result, fullpath, len);
	result[len] = 0;
	free(p);

	return result;
#endif
}

CM_END_EXTERN