#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "kernel/isr_wrapper.h"

#define PIC_BASE 32
#define PIC_IRQ(i) (PIC_BASE+(i))

void interrupts_setup();

typedef void (*interrupt_handler_t)(registers_t* regs);

int interrupts_install_handler(int i, interrupt_handler_t h);

#endif
