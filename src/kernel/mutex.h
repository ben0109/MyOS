#ifndef __MUTEX_H
#define __MUTEX_H

#include "types.h"

struct mutex;

typedef struct mutex mutex_t;

mutex_t* mutex_create();

void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

#endif

