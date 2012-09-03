#include <kernel/util.h>
#include <boot/multiboot.h>

#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/klog.h>
#include <kernel/vgatext.h>
#include <kernel/interrupts.h>
#include <kernel/pic.h>
#include <drivers/keyboard.h>
#include <kernel/timer.h>

extern uint32_t magic;
extern multiboot_info_t *mbd;

uint32_t trap80(uint32_t code, void* params);
 
void kmain()
{
	vgatext_clear();
	if ( magic != 0x2BADB002 )
	{
		kpanic("Wrong multiboot data -- abort");
	}

	kdebug("mem_lower: %d", mbd->mem_lower);
	kdebug("mem_upper: %d", mbd->mem_upper);
	kdebug("mmap:");

	uint32_t offset = 0;

	while (offset<mbd->mmap_length) {
		uint32_t address, length;
		multiboot_memory_map_t *entry = (multiboot_memory_map_t*)(mbd->mmap_addr + offset);

		address = entry->addr;
		length = entry->len;
		kdebug("  @%d: %d bytes, %s", address,length,(entry->type==1)?"RAM":"RESERVED");
		offset += entry->size + 4;
	}


	interrupts_setup();
	void* base = init_paging((void*)0x200000, (void*)0x1000000);

	sti();
	kmalloc_init(base, 0x20000);
	void* ptr1 = kmalloc(0x1000);
	void* ptr2 = kmalloc(0x8000);
	void* ptr3 = kmalloc(0x1000);
	kfree(ptr2);
	void* ptr4 = kmalloc(0x1000);
	kfree(ptr1);
	kfree(ptr3);
	kfree(ptr4);

	//timer_init();
	keyboard_init();

//	uint32_t r = trap80(0x4567,(void*)0x8901);
//	kdebug("r=%d",r);

	while(1);
}

