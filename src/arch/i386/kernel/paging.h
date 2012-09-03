#ifndef __PAGING_H
#define __PAGING_H

#define ENTRY_PRESENT			0x00000001
#define ENTRY_RW				0x00000002
#define ENTRY_USER				0x00000004
#define ENTRY_WRITETHROUGH		0x00000008
#define ENTRY_CACHEDISABLE		0x00000010
#define ENTRY_ACCESSED			0x00000020
#define ENTRY_DIRTY				0x00000040
#define ENTRY_PAGESIZE			0x00000080
#define ENTRY_G					0x00000100

#define ENTRY_AVAILABLE_MASK	0x00000e00
#define ENTRY_TABLE_MASK		0xfffff000
#define ENTRY_PAGE_MASK			0xfffff000

typedef struct {
	uint32_t entries[1024];
} page_table_t;

typedef struct page_directory {
	uint32_t entries[1024];
} page_directory_t;

// input=base free mem (after kernel etc.), top of ram
// returns the top of "mallocable" ram
void* init_paging(void* base, void* top);

void switch_page_directory(page_directory_t *new);

uint32_t* get_page(void* address, int create, page_directory_t* pd);

#endif

