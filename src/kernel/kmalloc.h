#ifndef __KMALLOC_H
#define __KMALLOC_H

#include "types.h"

#define KMALLOC_BYTE 0
#define KMALLOC_PAGE 1

void kmalloc_init(void* start, size_t size);
void* kmalloc(size_t size);
void kfree(void*);

void memset(void* ptr, uint8_t value, size_t size);
void memcpy(void* to, void* from, size_t size);

#endif
