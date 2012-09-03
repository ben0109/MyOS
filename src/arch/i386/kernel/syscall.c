#include "kernel/klog.h"
#include "kernel/interrupts.h"
#include "kernel/syscall.h"

#define MAX_SYSCALL 0x200

static void* syscalls[MAX_SYSCALL];

void syscall_handler(registers_t* registers);

void syscall_init()
{
	int i;
	for (i=0; i<MAX_SYSCALL; i++) {
		syscalls[i] = 0;
	}

	interrupts_install_handler(128, syscall_handler);
}


void syscall_handler(registers_t* registers)
{
	int i,ret;

	i = registers->eax;
	if (i>=MAX_SYSCALL) {
		return;
	}

	asm volatile (" \
		push %1; \
		push %2; \
		push %3; \
		push %4; \
		push %5; \
		call *%6; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx;"
	: "=a" (ret) 
	: "r" (registers->edi), "r" (registers->esi), "r" (registers->edx), "r" (registers->ecx), "r" (registers->ebx), "r" (syscalls[i]));

	// update stack with return value
	registers->eax = ret;
}

void syscall_install(int i, void* f)
{
	if ((i<0) || (i>=MAX_SYSCALL)) {
		kpanic("tried to install out of range syscall %d",i);
	}

	if (syscalls[i]!=0) {
		kpanic("duplicate syscall %d",i);
	}

	syscalls[i] = f;
}


