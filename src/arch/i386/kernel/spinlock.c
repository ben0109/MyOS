#include "kernel/kmalloc.h"
#include "kernel/spinlock.h"
#include "kernel/scheduler.h"

#define cli() asm volatile( "cli" )
#define sti() asm volatile( "sti" )

void spinlock_setup(spinlock_t* spinlock)
{
	*spinlock = 0;
}

void spinlock_lock(spinlock_t* spinlock)
{
	volatile uint32_t *ptr = spinlock;
	uint32_t ret;

	cli();
	do {
		asm volatile( 
			"lock; cmpxchgl %2,%1"
			: "=a" (ret), "+m" (*ptr)
			: "r" (1), "0" (0)
			: "memory");
	} while (ret!=0) ;
	sti();
}

void spinlock_unlock(spinlock_t* spinlock)
{
	*spinlock = 0;
}
