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

#include "affinity.h"
#include "header.h"
#include "memory.h"
#include "misc.h"

#include "debug.h"

#if defined(CM_SYS_WINDOWS)
void 
cm_affinity_init(cmAffinity *a) {
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION *start_processor_info = NULL;
	DWORD length = 0;
	b32 result  = GetLogicalProcessorInformation(NULL, &length);

	cm_zero_item(a);

	if (!result && GetLastError() == 122l /*ERROR_INSUFFICIENT_BUFFER*/ && length > 0) {
		start_processor_info = cast(SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)cm_alloc(cm_heap_allocator(), length);
		result = GetLogicalProcessorInformation(start_processor_info, &length);
		if (result) {
			SYSTEM_LOGICAL_PROCESSOR_INFORMATION *end_processor_info, *processor_info;

			a->is_accurate  = true;
			a->core_count   = 0;
			a->thread_count = 0;
			end_processor_info = cast(SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)cm_pointer_add(start_processor_info, length);

			for (processor_info = start_processor_info;
			     processor_info < end_processor_info;
			     processor_info++) {
				if (processor_info->Relationship == RelationProcessorCore) {
					isize thread = cm_count_set_bits(processor_info->ProcessorMask);
					if (thread == 0) {
						a->is_accurate = false;
					} else if (a->thread_count + thread > CM_WIN32_MAX_THREADS) {
						a->is_accurate = false;
					} else {
						CM_ASSERT(a->core_count <= a->thread_count &&
						          a->thread_count < CM_WIN32_MAX_THREADS);
						a->core_masks[a->core_count++] = processor_info->ProcessorMask;
						a->thread_count += thread;
					}
				}
			}
		}

		cm_free(cm_heap_allocator(), start_processor_info);
	}

	CM_ASSERT(a->core_count <= a->thread_count);
	if (a->thread_count == 0) {
		a->is_accurate   = false;
		a->core_count    = 1;
		a->thread_count  = 1;
		a->core_masks[0] = 1;
	}

}

void 
cm_affinity_destroy(cmAffinity *a) {
	cm_unused(a);
}

b32 
cm_affinity_set(cmAffinity *a, isize core, isize thread) {
	usize available_mask, check_mask = 1;
	CM_ASSERT(thread < cm_affinity_thread_count_for_core(a, core));

	available_mask = a->core_masks[core];
	for (;;) {
		if ((available_mask & check_mask) != 0) {
			if (thread-- == 0) {
				usize result = SetThreadAffinityMask(GetCurrentThread(), check_mask);
				return result != 0;
			}
		}
		check_mask <<= 1; // NOTE(bill): Onto the next bit
	}
}

isize 
cm_affinity_thread_count_for_core(cmAffinity *a, isize core) {
	CM_ASSERT(core >= 0 && core < a->core_count);
	return cm_count_set_bits(a->core_masks[core]);
}

#elif defined(CM_SYS_OSX)
void 
cm_affinity_init(cmAffinity *a) {
	usize count = 0;
	usize count_size = sizeof(count);

	a->is_accurate      = false;
	a->thread_count     = 1;
	a->core_count       = 1;
	a->threads_per_core = 1;

	if (sysctlbyname("hw.logicalcpu", &count, &count_size, NULL, 0) == 0) {
		if (count > 0) {
			a->thread_count = count;
			// Get # of physical cores
			if (sysctlbyname("hw.physicalcpu", &count, &count_size, NULL, 0) == 0) {
				if (count > 0) {
					a->core_count = count;
					a->threads_per_core = a->thread_count / count;
					if (a->threads_per_core < 1)
						a->threads_per_core = 1;
					else
						a->is_accurate = true;
				}
			}
		}
	}

}

void 
cm_affinity_destroy(cmAffinity *a) {
	cm_unused(a);
}

b32 
cm_affinity_set(cmAffinity *a, isize core, isize thread_index) {
	isize index;
	thread_t thread;
	thread_affinity_policy_data_t info;
	kern_return_t result;

	CM_ASSERT(core < a->core_count);
	CM_ASSERT(thread_index < a->threads_per_core);

	index = core * a->threads_per_core + thread_index;
	thread = mach_thread_self();
	info.affinity_tag = cast(integer_t)index;
	result = thread_policy_set(thread, THREAD_AFFINITY_POLICY, cast(thread_policy_t)&info, THREAD_AFFINITY_POLICY_COUNT);
	return result == KERN_SUCCESS;
}

isize 
cm_affinity_thread_count_for_core(cmAffinity *a, isize core) {
	CM_ASSERT(core >= 0 && core < a->core_count);
	return a->threads_per_core;
}

#elif defined(CM_SYS_LINUX)
// IMPORTANT TODO(bill): This cmAffinity stuff for linux needs be improved a lot!
// NOTE(zangent): I have to read /proc/cpuinfo to get the number of threads per core.
#include <stdio.h>

void 
cm_affinity_init(cmAffinity *a) {
	b32   accurate = true;
	isize threads = 0;

	a->thread_count     = 1;
	a->core_count       = sysconf(_SC_NPROCESSORS_ONLN);
	a->threads_per_core = 1;


	if(a->core_count <= 0) {
		a->core_count = 1;
		accurate = false;
	}

	// Parsing /proc/cpuinfo to get the number of threads per core.
	// NOTE(zangent): This calls the CPU's threads "cores", although the wording
	// is kind of weird. This should be right, though.

	FILE* cpu_info = fopen("/proc/cpuinfo", "r");

	if (cpu_info != NULL) {
		for (;;) {
			// The 'temporary char'. Everything goes into this char,
			// so that we can check against EOF at the end of this loop.
			char c;

#define AF__CHECK(letter) ((c = getc(cpu_info)) == letter)
			if (AF__CHECK('c') && AF__CHECK('p') && AF__CHECK('u') && AF__CHECK(' ') &&
			    AF__CHECK('c') && AF__CHECK('o') && AF__CHECK('r') && AF__CHECK('e') && AF__CHECK('s')) {
				// We're on a CPU info line.
				while (!AF__CHECK(EOF)) {
					if (c == '\n') {
						break;
					} else if (c < '0' || '9' > c) {
						continue;
					}
					threads = threads * 10 + (c - '0');
				}
				break;
			} else {
				while (!AF__CHECK('\n')) {
					if (c==EOF) {
						break;
					}
				}
			}
			if (c == EOF) {
				break;
			}
#undef AF__CHECK
		}

		fclose(cpu_info);
	}

	if (threads == 0) {
		threads  = 1;
		accurate = false;
	}

	a->threads_per_core = threads;
	a->thread_count = a->threads_per_core * a->core_count;
	a->is_accurate = accurate;

}

void 
cm_affinity_destroy(cmAffinity *a) {
	cm_unused(a);
}

b32 
cm_affinity_set(cmAffinity *a, isize core, isize thread_index) {
	return true;
}

isize 
cm_affinity_thread_count_for_core(cmAffinity *a, isize core) {
	CM_ASSERT(0 <= core && core < a->core_count);
	return a->threads_per_core;
}
#else
#error TODO(bill): Unknown system
#endif
