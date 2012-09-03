#include "klog.h"
#include "kmalloc.h"

#define MALLOC_DEBUG
#define MALLOC_MAGIC 0xC0DEC0DE

#define MALLOC_USED  1
#define MALLOC_FIRST 2
#define MALLOC_LAST  4

typedef struct __malloc_block {
#ifdef MALLOC_DEBUG
	uint32_t magic;
#endif
	uint32_t flags;
	uint32_t size;
	uint8_t data[];
} malloc_block_t;

typedef struct {
#ifdef MALLOC_DEBUG
	uint32_t magic;
#endif
	malloc_block_t* backlink;
} malloc_block_end_t;

#define BLOCK_OVERHEAD (sizeof(malloc_block_t) + sizeof(malloc_block_end_t))
#define PAYLOAD_SIZE(block) ((block)->size - BLOCK_OVERHEAD)
#define BLOCK_NEXT(block) ((malloc_block_t*)(((uint32_t)block) + block->size))
#define BLOCK_END(block) ((malloc_block_end_t*)(((uint32_t)BLOCK_NEXT(block)) - sizeof(malloc_block_end_t)))
#define BLOCK_PREVIOUS(block) (((malloc_block_end_t*)(((uint32_t)block) - sizeof(malloc_block_end_t)))->backlink)




malloc_block_t* first_block;

void setup_block(malloc_block_t* block, uint32_t flags, uint32_t size)
{
#ifdef MALLOC_DEBUG
	block->magic = MALLOC_MAGIC;
#endif
	block->flags = flags;
	block->size = size;

	malloc_block_end_t* block_end = BLOCK_END(block);
#ifdef MALLOC_DEBUG
	block_end->magic = MALLOC_MAGIC;
#endif
	block_end->backlink = block;
}

#ifdef MALLOC_DEBUG
void check_block(malloc_block_t* block)
{
	if (  (block->magic!=MALLOC_MAGIC)
		| (BLOCK_END(block)->magic!=MALLOC_MAGIC)
		| (BLOCK_END(block)->backlink!=block))
	{
		kpanic("corrupted malloc data");
	}
	if ((block->flags&MALLOC_LAST)==0) {
		malloc_block_t* next = BLOCK_NEXT(block);
		if ((next->magic!=MALLOC_MAGIC) || (next->flags&MALLOC_FIRST)) {
			kpanic("corrupted malloc chain");
		}
	}
	if ((block->flags&MALLOC_FIRST)==0) {
		malloc_block_t* prev = BLOCK_PREVIOUS(block);
		if ((prev->magic!=MALLOC_MAGIC) || (prev->flags&MALLOC_LAST)) {
			kpanic("corrupted malloc chain");
		}
	}
}
#else
#define check_block(block)
#endif

#if 0
void kmalloc_print()
{
	malloc_block_t* ptr = first_block;
	while (1) {
		kdebug("  @%d, size=%d, flags=%b", ptr, ptr->size, ptr->flags);
		if ((ptr->flags&MALLOC_LAST)!=0) {
			break;
		}
		ptr = BLOCK_NEXT(ptr);
	}
}
#else
#define kmalloc_print()
#endif

void kmalloc_init(void* base, size_t size)
{
	kdebug("< kmalloc_init %d, %d", base, size);
	first_block = (malloc_block_t*)base;
	setup_block(first_block, MALLOC_FIRST|MALLOC_LAST, size);
	kmalloc_print();
	kdebug(">");
}

void* kmalloc(size_t payload)
{
	malloc_block_t* block;
	malloc_block_t* ptr;

	kdebug("< kmalloc %d", payload);
	
	block = 0;
	ptr = first_block;
	while (1) {
		check_block(ptr);
		// free and big enough ?
		if (((ptr->flags&MALLOC_USED)==0) && (PAYLOAD_SIZE(ptr)>=payload)) {
			// better than the current fit (if any) ?
			if ((block==0) || (ptr->size<block->size)) {
				block = ptr;
			}
		}
		if (ptr->flags&MALLOC_LAST) {
			break;
		}
		ptr = BLOCK_NEXT(ptr);
	}

	if (ptr==0) {
		kpanic("could not allocate %d", payload);
		return 0;
	}

	int split_block = (block->size > (payload+BLOCK_OVERHEAD));
	if (split_block) {
		int last = block->flags&MALLOC_LAST;
		size_t size     = payload + BLOCK_OVERHEAD;
		size_t rem_size = block->size - size;
		setup_block(block, (block->flags&~MALLOC_LAST)|MALLOC_USED, size);
		setup_block(BLOCK_NEXT(block), last, rem_size);
	} else {
		block->flags |= MALLOC_USED;
	}
	kmalloc_print();
	kdebug(">");
	return (void*)block->data;
}

void kfree(void* address)
{
	kdebug("< kfree %d", address);

	// start of block
	malloc_block_t* block = address - (uint32_t)&(((malloc_block_t*)0)->data);
	check_block(block);
	
	int size = block->size;
	int flags = block->flags & ~MALLOC_USED;

	// if next block is free, merge with it
	if ((block->flags&MALLOC_LAST)==0) {
		malloc_block_t* next = BLOCK_NEXT(block);
		if ((next->flags&MALLOC_USED)==0) {
			size += next->size;
			flags |= (next->flags&MALLOC_LAST);
		}
	}
	// if previous block is free, merge with it
	if ((block->flags&MALLOC_FIRST)==0) {
		malloc_block_t* prev = BLOCK_PREVIOUS(block);
		if ((prev->flags&MALLOC_USED)==0) {
			size += prev->size;
			flags |= (prev->flags&MALLOC_FIRST);
			block = prev;
		}
	}

	setup_block(block, flags, size);
	kmalloc_print();
	kdebug(">");
}


void memset(void* ptr, uint8_t value, size_t size)
{
	size_t i;
	uint8_t* ptrb = (uint8_t*)ptr;
	for (i=0; i<size; i++) {
		*ptrb++ = value;
	}
}

void memcpy(void* to, void* from, size_t size)
{
	uint32_t i;
	uint8_t* from8 = from;
	uint8_t* to8   = to;

	for (i=0; i<size; i++) {
		*to8++ = *from8++;
	}
}
