#include "kernel/klog.h"
#include "kernel/kmalloc.h"
#include "kernel/interrupts.h"
#include "kernel/task.h"
#include "kernel/scheduler.h"
#include "kernel/spinlock.h"

/* running tasks are put in a ring */
typedef struct __task_ring_element {
	struct __task_ring_element* previous;
	struct __task_ring_element* next;
	task_t* task;
} task_ring_element_t;

static task_ring_element_t *queue_head;
static spinlock_t queue_lock;
task_t* current_task;



void scheduler_init(task_t* reaper_task)
{
	queue_head = kmalloc(sizeof(task_ring_element_t));
	queue_head->next = queue_head;
	queue_head->previous = queue_head;
	queue_head->task = reaper_task;

	current_task = reaper_task;

	spinlock_setup(&queue_lock);
}

#define lock_queue() spinlock_lock(&queue_lock)
#define unlock_queue() spinlock_unlock(&queue_lock)

void sched_schedule(task_t* task)
{
	// task is running: nothing to do
	if (task->state==TASK_RUNNING) {
		return;
	}

	lock_queue();

	// put it at the end of queue (juste 'before' head)
	task_ring_element_t *e = kmalloc(sizeof(task_ring_element_t));
	e->previous = queue_head->previous;
	queue_head->previous = e;
	e->next = queue_head;

	unlock_queue();
}

void remove_from_queue(task_ring_element_t* e)
{
	e->next->previous = e->previous;
	e->previous->next = e->next;
	unlock_queue();
	kfree(e);
}

void sched_unschedule_current(int new_state)
{
	current_task->state = new_state;

	// task is not running: nothing more to do
	if (current_task->state!=TASK_RUNNING) {
		return;
	}

	task_t* task;

	// remove head
	lock_queue();
	task_ring_element_t* old_head = queue_head;
	queue_head = queue_head->next;
	remove_from_queue(old_head);
	task = queue_head->task;
	unlock_queue();

	// yield
	arch_task_save_current();
	arch_task_restore(task);
}

void sched_unschedule(task_t* task, int new_state)
{
	if (task==current_task) {
		kpanic("sched_unschedule: trie to unschedule current task");
	}

	task->state = new_state;

	// task is not running: nothing more to do
	if (task->state!=TASK_RUNNING) {
		return;
	}

	task_ring_element_t *ptr = queue_head->next;

	while (ptr!=queue_head) {
		if (ptr->task==task) {
			lock_queue();
			remove_from_queue(ptr);
			unlock_queue();
			return;
		}
		ptr = ptr->next;
	};

	// not found
	kerror("sched_unschedule: unknown task");
}

void sched_yield()
{
	task_t* task;

	lock_queue();
	queue_head = queue_head->next;
	task = queue_head->task;
	unlock_queue();

	arch_task_save_current();
	arch_task_restore(task);
}

