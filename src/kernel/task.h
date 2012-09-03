#ifndef __TASK_H
#define __TASK_H

#include <stdint-gcc.h>
#include "kernel/arch-task.h"

#define TASK_RUNNING 0
#define TASK_WAITING 1
#define TASK_ZOMBIE 2

typedef struct __arch_task_struct arch_task_t;

struct __process_struct;
struct __task_struct;
typedef struct __process_struct process_t;
typedef struct __task_struct task_t;

struct __process_struct {
	uint32_t stack_size;
	task_t* tasks;
};

struct __task_struct {
	process_t* process;
	task_t* previous;
	task_t* next;

	uint32_t* stack_top;
	uint32_t* stack_ptr;
	int state;
	arch_task_t arch_task;
};


typedef struct __task_list {
	task_t *head;
	struct __task_list *tail;
} task_list_t;

#define task_list_empty(tl) ((tl)==0)
void task_list_push(task_list_t** tl, task_t* t);
void task_list_pop(task_list_t** tl, task_t** r);
	


void arch_task_setup(task_t* task);
void arch_task_copy(task_t* dst, task_t* src);

void arch_task_save_current();
void arch_task_restore(task_t* task);

// for fork
void arch_task_set_return_point(task_t* task, void* ptr);

void task_reaper_task();
task_t* task_create(process_t* process);
task_t* task_copy(task_t* task);
void task_end(task_t* task);

int fork();


#endif
