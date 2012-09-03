#include "kernel/spinlock.h"
#include "kernel/tasklet.h"

struct __tasklet_list {
	void (*tasklet)(void);
	struct __tasklet_list* next;
};

typedef struct __tasklet_list tasklet_list_t;


static spinlock_t tasklet_lock;
static tasklet_list_t* scheduled_tasklets = 0;

void tasklet_init()
{
	spinlock_setup(&tasklet_lock);
}

void tasklet_schedule(void (*tasklet)(void))
{
	spinlock_lock(&tasklet_lock);

	tasklet_list_t* head = (tasklet_list_t*)kmalloc(sizeof(tasklet_list_t));
	head->tasklet = tasklet;
	head->next = scheduled_tasklets;
	scheduled_tasklets = head;

	spinlock_unlock(&tasklet_lock);
}

void tasklet_run_scheduled()
{
	tasklet_list_t* ptr;

	spinlock_lock(&tasklet_lock);
	ptr = scheduled_tasklets;
	scheduled_tasklets = 0;
	spinlock_unlock(&tasklet_lock);

	while (ptr) {
		tasklet_list_t* next = ptr->next;
		ptr->tasklet();
		kfree(ptr);
		ptr = next;
	}
}
