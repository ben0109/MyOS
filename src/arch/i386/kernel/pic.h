#ifndef __PIC_H
#define __PIC_H

#include <stdint-gcc.h>

void pic_init(int offset);
void pic_enable_line(int line);
void pic_disable_line(int line);

uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
void pic_send_eoi(int line);



#endif
