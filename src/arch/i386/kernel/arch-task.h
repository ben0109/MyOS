#ifndef __TASK_ARCH_H
#define __TASK_ARCH_H

#include "types.h"
#include "kernel/paging.h"

struct __arch_task_struct {
	uint32_t* *sp;
	uint32_t* kernel_sp;
	page_directory_t* *page_directory;
};

#endif
