#include "kernel/kmalloc.h"
#include "kernel/scheduler.h"
#include "kernel/semaphore.h"

semaphore_t* semaphore_create()
{
	// TODO do that in process memory rather than kernel 
	semaphore_t* semaphore = kmalloc(sizeof(semaphore_t));

	spinlock_setup(&semaphore->lock);
	semaphore->waiting_tasks = 0;

	return semaphore;
}

void semaphore_wait(semaphore_t* semaphore)
{
	// lock semaphore
	spinlock_lock(&semaphore->lock);

	int wait = (--semaphore->value<0);

	// if blocked, add the current tasks to the "waiting tasks" list
	if (wait) {
		task_list_t* list = kmalloc(sizeof(task_list_t));
		list->head = current_task;
		list->tail = semaphore->waiting_tasks;
		semaphore->waiting_tasks = list;
	}

	// unlock semaphore
	spinlock_unlock(&semaphore->lock);

	// if blocked, stop current thread
	if (wait) {
		sched_unschedule(current_task, TASK_WAITING);
		sched_yield();
	}
}

void semaphore_signal(semaphore_t* semaphore)
{
	// lock semaphore
	spinlock_lock(&semaphore->lock);

	// wake up first waiting task, if any
	if (semaphore->waiting_tasks) {
		task_t *task;
		task_list_pop(&semaphore->waiting_tasks, &task);
		sched_schedule(task);
	}

	// unlock semaphore
	spinlock_unlock(&semaphore->lock);
}

