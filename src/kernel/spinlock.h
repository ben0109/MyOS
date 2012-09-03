#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include "types.h"

typedef uint32_t spinlock_t;

void spinlock_setup(spinlock_t* spinlock);
void spinlock_lock(spinlock_t* spinlock);
void spinlock_unlock(spinlock_t* spinlock);

#endif

