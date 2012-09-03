#ifndef __TASKLET_H
#define __TASKLET_H

typedef void (*f)(void) tasklet_t;

void tasklet_schedule(void (*tasklet)());
void tasklet_run_scheduled();

#endif
