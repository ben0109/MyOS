#include "kernel/util.h"
#include "kernel/klog.h"
#include "kernel/interrupts.h"
#include "kernel/pic.h"
#include "kernel/timer.h"

#define IRQ0 32

uint32_t tick = 0;

static void irq_handler(registers_t*);

void timer_init()
{
	uint32_t divisor = 1193180 / 100; // for 100 Hz

	// Send the command byte.
	io_outb(0x43, 0x36);

	// Send the frequency divisor.
	io_outb(0x40, (uint8_t)((divisor>>0)&0xFF));
	io_outb(0x40, (uint8_t)((divisor>>8)&0xFF));

	// enable irq
	interrupts_install_handler(PIC_IRQ(0), irq_handler);
	pic_enable_line(0);
}

static void irq_handler(registers_t* registers)
{
	registers->ds &= 0xffff;
	tick++;
	kdebug("tick %d",tick);
}

