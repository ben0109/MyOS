#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "types.h"
#include "kernel/spinlock.h"
#include "kernel/task.h"

typedef struct {
	spinlock_t lock;
	int value;
	task_list_t* waiting_tasks;
} semaphore_t;

semaphore_t* semaphore_create();
void semaphore_acquire(semaphore_t* semaphore);
void semaphore_release(semaphore_t* semaphore);

#endif

