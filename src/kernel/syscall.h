#ifndef __SYSCALL_H
#define __SYSCALL_H

void syscall_init();
void syscall_install(int i, void* f);

#endif
