#ifndef __GDT_H
#define __GDT_H

#include "types.h"

#define GDT_INDEX_TSS		1
#define GDT_INDEX_KERN_CODE	2
#define GDT_INDEX_KERN_DATA	3
#define GDT_INDEX_USER_CODE	4
#define GDT_INDEX_USER_DATA	5

#define SEGMENT_TSS			(GDT_INDEX_TSS<<3)
#define SEGMENT_KERN_CODE	(GDT_INDEX_KERN_CODE<<3)
#define SEGMENT_KERN_DATA	(GDT_INDEX_KERN_DATA<<3)
#define SEGMENT_USER_CODE	(GDT_INDEX_USER_CODE<<3)
#define SEGMENT_USER_DATA	(GDT_INDEX_USER_DATA<<3)

#define SEGMENT_USER_ACCESS 3

struct gdt_entry_struct
{
   uint16_t limit_low;
   uint16_t base_low;
   uint8_t  base_middle;
   uint8_t  access;
   uint8_t  granularity;
   uint8_t  base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

void gdt_set_entry(int index, uint32_t base, size_t limit, int access, int granularity);

#endif
