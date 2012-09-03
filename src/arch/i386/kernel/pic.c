#include "kernel/util.h"
#include "kernel/pic.h"

#define PIC1          0x20      /* IO base address for master PIC */
#define PIC2          0xA0      /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA     (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA     (PIC2+1)

#define PIC_EOI       0x20      /* End-of-interrupt command code */
#define PIC_READ_IRR  0x0a      /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR  0x0b      /* OCW3 irq service next CMD read */

void pic_init(int offset)
{
	io_outb(PIC1_COMMAND, 0x11);     // starts the initialization sequence (in cascade mode)
	io_wait();
	io_outb(PIC2_COMMAND, 0x11);
	io_wait();
	io_outb(PIC1_DATA, offset);      // ICW2: Master PIC vector offset
	io_wait();
	io_outb(PIC2_DATA, offset+8);    // ICW2: Slave PIC vector offset
	io_wait();
	io_outb(PIC1_DATA, 4);           // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	io_outb(PIC2_DATA, 2);           // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	io_outb(PIC1_DATA, 0x01);        // 8086/88 (MCS-80/85) mode
	io_wait();
	io_outb(PIC2_DATA, 0x01);
	io_wait();
 
	io_outb(PIC1_DATA, 0xff);        // disable all
	io_outb(PIC2_DATA, 0xff);
}

void pic_disable_line(int line)
{
    uint16_t port;
    uint8_t value;
 
    if(line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = io_inb(port) | (1 << line);
    io_outb(port, value);        
}
 
void pic_enable_line(int line)
{
    uint16_t port;
    uint8_t value;
 
    if (line<8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = io_inb(port) & ~(1 << line);
    io_outb(port, value);        
}

 
static uint16_t pic_get_irq_reg(int ocw3)
{
	io_outb(PIC1_COMMAND, ocw3);
	io_outb(PIC2_COMMAND, ocw3);
	return (io_inb(PIC2_COMMAND) << 8) | io_inb(PIC1_COMMAND);
}
 
/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
	return pic_get_irq_reg(PIC_READ_IRR);
}
 
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
	return pic_get_irq_reg(PIC_READ_ISR);
}

void pic_send_eoi(int line)
{
	uint16_t port = (line<8) ? PIC1_COMMAND : PIC2_COMMAND;
	io_outb(port,PIC_EOI);
}










