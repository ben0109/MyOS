#include "kernel/kmalloc.h"
#include "kernel/task.h"
#include "kernel/scheduler.h"


// dead tasks, that are not freed yet
task_list_t* zombie_tasks = 0;

void task_list_push(task_list_t** tl, task_t* t)
{
	task_list_t* new = (task_list_t*)kmalloc(sizeof(task_list_t));
	new->head = t;
	new->tail = *tl;
	*tl = new;
}
void task_list_pop(task_list_t** tl, task_t** r)
{
	task_list_t* tail = (*tl)->tail;
	*r = (*tl)->head;
	kfree(tl);
	*tl = tail;
}

void task_end(task_t* task)
{
	// add to the list of zombies
	task_list_t* old_zombie_tasks = zombie_tasks;

	zombie_tasks = kmalloc(sizeof(task_list_t));
	zombie_tasks->head = task;
	zombie_tasks->tail = old_zombie_tasks;

	sched_unschedule(task, TASK_ZOMBIE);
}

void task_reap(task_t* task)
{
	if (task->previous) {
		task->previous->next = task->next;
	} else {
		task->process->tasks = task->next;
	}

	if (task->next) {
		task->next->previous = task->previous;
	}

	kfree(task);
}

void task_reaper_task()
{
	while (1) {
		while (zombie_tasks) {
			task_list_t* old_zombie_tasks = zombie_tasks;
			zombie_tasks = zombie_tasks->tail;

			task_reap(old_zombie_tasks->head);
			kfree(old_zombie_tasks);
		}
		sched_yield();
	}
}

task_t* task_create(process_t* process)
{
	int stack_size = process->stack_size;
	void* stack_bottom = kmalloc(stack_size);
	task_t* task = (task_t*)kmalloc(sizeof(task));

	/* setup task struct */
	task->process = process;
	task->stack_top = stack_bottom + stack_size;
	task->stack_ptr = task->stack_top;
	task->state = TASK_WAITING;

	/* setup arch specific data */
	arch_task_setup(task);

	/* put it at the beginning of the process' task list */
	task->previous = 0;
	task->next = process->tasks;
	process->tasks = task;

	return task;
}

task_t* task_copy(task_t* task)
{
	process_t* process = task->process;

	task_t* copy = task_create(process);
	int stack_depth = task->stack_top - task->stack_ptr;

	copy->stack_ptr = copy->stack_top - stack_depth;
	memcpy(copy->stack_ptr, task->stack_ptr, stack_depth);

	/* setup arch specific data */
	arch_task_copy(copy,task);

	return copy;
}

int fork_child_return_point()
{
	return 1;
}

int fork()
{
	arch_task_save_current();

	task_t* child = task_copy(current_task);
	arch_task_set_return_point(child, &fork_child_return_point);
	sched_schedule(child);

	return 0;
}

