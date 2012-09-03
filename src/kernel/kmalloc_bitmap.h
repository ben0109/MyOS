#ifndef __KMALLOC_H
#define __KMALLOC_H

#include <stdint-gcc.h>

void* kmalloc(uint32_t size);
void kfree(void* address);

void memcpy(void* to, void* from, uint32_t size);

#endif

