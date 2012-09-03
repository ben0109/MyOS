#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "kernel/task.h"

extern task_t* current_task;

void sched_schedule(task_t *task);
void sched_unschedule(task_t *task, int new_state);
void sched_yield();

#endif

