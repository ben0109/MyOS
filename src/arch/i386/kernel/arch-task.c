#include "kernel/kmalloc.h"
#include "kernel/gdt.h"
#include "kernel/task.h"
#include "kernel/scheduler.h"

struct tss {
	uint32_t tss_back_link; // bits 16-31 must be 0
	uint32_t esp0;
	uint32_t ss0;           // bits 16-31 must be 0
	uint32_t esp1;
	uint32_t ss1;           // bits 16-31 must be 0
	uint32_t esp2;
	uint32_t ss2;           // bits 16-31 must be 0

	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;

	uint32_t es;            // bits 16-31 must be 0
	uint32_t cs;            // bits 16-31 must be 0
	uint32_t ss;            // bits 16-31 must be 0
	uint32_t ds;            // bits 16-31 must be 0
	uint32_t fs;            // bits 16-31 must be 0
	uint32_t gs;            // bits 16-31 must be 0

	uint32_t ldt;           // bits 16-31 must be 0
	uint16_t trap;          // bit 0 is for trap (should be 0), and bits 1-15 must be 0
	uint16_t iomap_base;
} __attribute__((packed));

typedef struct tss tss_t;

tss_t tss;

void tss_setup(void* kernel_stack_pointer)
{
	// clear it all
	memset(&tss,0,sizeof(tss_t));

	// setup system stack
	tss.esp0 = (uint32_t)kernel_stack_pointer;
	tss.ss0  = SEGMENT_KERN_DATA;

	// setup cs:eip and ss:esp for startup
/*
	tss->cs  = SEGMENT_KERN_CODE|SEGMENT_USER_ACCESS;
	tss->ds  = SEGMENT_KERN_DATA|SEGMENT_USER_ACCESS;
	tss->es  = SEGMENT_KERN_DATA|SEGMENT_USER_ACCESS;
	tss->fs  = SEGMENT_KERN_DATA|SEGMENT_USER_ACCESS;
	tss->gs  = SEGMENT_KERN_DATA|SEGMENT_USER_ACCESS;
	tss->ss  = SEGMENT_KERN_DATA|SEGMENT_USER_ACCESS;
*/

	uint32_t base = (uint32_t)&tss;
	uint32_t limit = base+sizeof(tss_t);

	gdt_set_entry(GDT_INDEX_TSS, base, limit, 0xe9, 0x00);

	asm volatile ( "ltr %%ax" :: "a"(SEGMENT_TSS|3) );
}

void arch_task_setup(task_t* task)
{
	task++;
}

void arch_task_copy(task_t* dst, task_t* src)
{
	src++;
	dst++;
}

void arch_task_save_current()
{
	// save kernel sp
	current_task->arch_task.kernel_sp = (void*)tss.esp0;

	// save registers on stack, and save sp 
	asm volatile (
		"pushal;"
		"movl %%esp,%0"
	: "=r"(current_task->arch_task.sp) );
}

void arch_task_restore(task_t* task)
{
	current_task = task;

	// restore sp, pop registers
	asm volatile (
		"movl %0,%%esp;"
		"popal;"
	:
	: "r"(current_task->stack_ptr) );

	// save kernel sp
	tss.esp0 = (uint32_t)current_task->arch_task.kernel_sp;

	// and continue (in task)
}

void arch_task_set_return_point(task_t* task, void* ptr)
{
	task->stack_ptr[12] = (uint32_t)ptr;
}

void arch_task_start_user_code(void* user_sp, void* start_eip)
{
	asm volatile(
		"cli;"

		"mov   %0, %%ax;"	// change segments
		"mov   %%ax, %%ds;"
		"mov   %%ax, %%es;"
		"mov   %%ax, %%fs;"
		"mov   %%ax, %%gs;"

		"pushl %0;"			// push stack address
		"mov   %1,%%eax;"
		"pushl %%eax;"

		"pushf;"			// push flags, with interrupts enabled
		"popl  %%eax;"
		"orl   $0x200,%%eax;"
		"pushl %%eax;"

		"pushl %2;"			// push start code address
		"pushl %3;"

		"iret;"				// go

	: : "g"(SEGMENT_USER_DATA|SEGMENT_USER_ACCESS), "g"(user_sp),
		"g"(SEGMENT_USER_CODE|SEGMENT_USER_ACCESS), "g"(start_eip));
}

