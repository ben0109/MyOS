#include "kernel/klog.h"
#include "kernel/kmalloc.h"
#include "kernel/interrupts.h"
#include "kernel/paging.h"

int npages;
uint32_t* frame_used;

page_directory_t* kernel_page_directory;
page_directory_t* current_page_directory;

#define FRAME_TEST(i) (frame_used[(i)>>5] & (1<<((i)&0x1f)))
#define FRAME_MARK(i) frame_used[(i)>>5] |= 1<<((i)&0x1f)
#define FRAME_CLEAR(i) frame_used[(i)>>5] &= ~(1<<((i)&0x1f))

void page_fault_handler(registers_t* registers);

uint32_t build_page_table_entry(uint32_t physical_page_index, int is_kernel, int is_rw)
{
	FRAME_MARK(physical_page_index);
	return ENTRY_PRESENT
		| ((is_rw) ? ENTRY_RW : 0)
		| ((is_kernel) ? 0 : ENTRY_USER)
		| (physical_page_index<<12);
}

void* get_physical_address(void* virtual_address)
{
	uint32_t global_offset = (uint32_t)virtual_address;

	// 0x1000 bytes per page
	int page_index = global_offset>>12;
	int page_offset = global_offset & 0xfff;

	// 0x400 pages per table
	int table_index = page_index>>10;
	int page_table_offset = page_index&0x3ff;

	uint32_t pde = current_page_directory->entries[table_index];
	page_table_t* table = (page_table_t*)(pde & ENTRY_TABLE_MASK);

	uint32_t pte = table->entries[page_table_offset];
	void* frame_base = (void*)(pte & ENTRY_PAGE_MASK);
	
	return frame_base + page_offset;
}

void* init_paging(void* base, void* top)
{
	uint32_t i;
	uint32_t kheap_ptr = (uint32_t)base;
	
	// allocate memory for the pages bitmap, filled with 0 (free)
	npages = ((uint32_t)top)/0x1000;
	int bitmap_size = 4*(npages/0x20);
	frame_used = (uint32_t*)kheap_ptr; kheap_ptr += bitmap_size;
	memset(frame_used,0,bitmap_size);

	// allocate the page directory at a page-aligned address, filled with 0 (tables not present)
	kheap_ptr = (kheap_ptr+0xfff) & ~ 0xfff;
	kernel_page_directory = (page_directory_t*)kheap_ptr; kheap_ptr += sizeof(page_directory_t);
	memset(kernel_page_directory,0,sizeof(page_directory_t));

	// allocate a page table at a page-aligned address, filled with 0 (pages not present)
	kheap_ptr = (kheap_ptr+0xfff) & ~ 0xfff;
	page_table_t* page_table = (page_table_t*)kheap_ptr; kheap_ptr += sizeof(page_table_t);
	memset(page_table,0,sizeof(page_table_t));

	// identity map the first 4MB of memory (all 1024 entries of the first page table)
	kernel_page_directory->entries[0] = ((uint32_t)page_table) | ENTRY_PRESENT | ENTRY_RW;
	for (i=0; i<0x400; i++) {
		page_table->entries[i] = build_page_table_entry(i, 0, 0);
	}

	// register page fault handler
	interrupts_install_handler(14, page_fault_handler);

	// load pdt
	switch_page_directory(kernel_page_directory);

	return (void*)kheap_ptr;
}

void switch_page_directory(page_directory_t* pd)
{
	asm volatile ( "cli" );

	current_page_directory = pd;

	// setup page directory and enable paging
	uint32_t cr0;
	asm volatile("mov %0, %%cr3":: "r"(&pd->entries));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));

	asm volatile ( "sti" );
}

int find_free_frame()
{
	int i,j;
	for (i=0; i<npages/0x20; i++) {
		if (frame_used[i]!=0xffffffff) {
			for (j=0; j<32; j++) {
				if ((frame_used[i<<5]&(1<<j))==0) {
					return (i<<5)+j;
				}
			}
		}
	}
	kpanic("no free frames");
	return -1;
}

uint32_t* get_page(void* address, int create, page_directory_t* pd)
{
	int page_index = ((uint32_t)address)/0x1000;
	int table_index = page_index/0x400;

	page_table_t *pt = (void*)(pd->entries[table_index] & ENTRY_TABLE_MASK);
	if (pt==0) {
		if (!create) {
			return 0;
		}

		pt = (page_table_t*)kmalloc(sizeof(page_table_t));
		memset(pt,0,sizeof(page_table_t));
		pd->entries[table_index] = ((uint32_t)pt) | ENTRY_PRESENT | ENTRY_RW | ENTRY_USER;
	}

	return &pt->entries[page_index & 0x3ff];
}

void alloc_frame(uint32_t *page, int is_kernel, int is_rw)
{
	if (*page & ENTRY_PRESENT) {
		return;
	}

	uint32_t i = find_free_frame();
	*page = build_page_table_entry(i, is_kernel, is_rw);
}

void free_frame(uint32_t *page)
{
	if ((*page & ENTRY_PRESENT)==0) {
		return;
	}

	FRAME_CLEAR(*page>>12);
	*page &= ~(ENTRY_PAGE_MASK|ENTRY_PRESENT);
}


void page_fault_handler(registers_t* registers)
{
	registers->eax &= ~0;
}


