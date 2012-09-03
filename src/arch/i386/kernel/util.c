#include "kernel/util.h"

uint8_t io_inb(uint16_t port)
{
	uint8_t v;
	__asm__ ( "inb %1,%0" : "=a"(v) : "Nd"(port) );
	return v;
}

void io_outb(uint16_t port, uint8_t value)
{
	__asm__ ( "outb %0,%1" :: "a"(value),"Nd"(port) );
}

void io_wait()
{
	/* empty */
}

