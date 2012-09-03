#include "kernel/kmalloc.h"
#include "kernel/spinlock.h"
#include "kernel/mutex.h"
#include "kernel/scheduler.h"

#define MUTEX_FREE 0
#define MUTEX_LOCKED 1

struct mutex {
	spinlock_t lock;
	int state;
	task_list_t* waiting_tasks;
};

mutex_t* mutex_create()
{
	mutex_t* mutex = kmalloc(sizeof(mutex_t));

	spinlock_setup(&mutex->lock);
	mutex->state = MUTEX_FREE;
	mutex->waiting_tasks = 0;

	return mutex;
}

int mutex_is_locked(mutex_t* mutex)
{
	return mutex->lock!=MUTEX_FREE;
}

/* return TRUE if success */
int mutex_try_lock(mutex_t* mutex)
{
	spinlock_lock(&mutex->lock);

	int r = mutex->lock==MUTEX_FREE;
	if (r) {
		mutex->state = MUTEX_LOCKED;
	}

	spinlock_unlock(&mutex->lock);
	return r;
}

/* looping wait locking, with yield */
void mutex_yield_wait(mutex_t* mutex)
{
	while (1) {
		if (mutex_try_lock(mutex)) {
			break;
		}
		sched_yield();
	}
}

void mutex_unlock(mutex_t* mutex)
{
	mutex->lock = MUTEX_FREE;
}

