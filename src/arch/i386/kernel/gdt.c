#include "kernel/gdt.h"

extern gdt_entry_t* gdt_entries;

void gdt_set_entry(int index, uint32_t base, size_t limit, int access, int granularity)
{
   gdt_entries[index].base_low    = (base & 0xFFFF);
   gdt_entries[index].base_middle = (base >> 16) & 0xFF;
   gdt_entries[index].base_high   = (base >> 24) & 0xFF;

   gdt_entries[index].limit_low   = (limit & 0xFFFF);
   gdt_entries[index].granularity = (limit >> 16) & 0x0F;

   gdt_entries[index].granularity |= granularity & 0xF0;
   gdt_entries[index].access      = access;
}

