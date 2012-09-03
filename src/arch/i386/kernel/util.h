#ifndef __UTIL_H
#define __UTIL_H

#include "types.h"

#define cli() asm volatile ( "cli" )
#define sti() asm volatile ( "sti" )

uint8_t io_inb(uint16_t port);
void io_outb(uint16_t port, uint8_t value);
void io_wait();

#endif
