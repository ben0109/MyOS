#include "types.h"
#include "kernel/klog.h"
#include "kernel/segments.h"
#include "kernel/isr_wrapper.h"
#include "kernel/pic.h"
#include "drivers/keyboard.h"
#include "kernel/interrupts.h"

#define MAX_INT_NO 0x81

#define TYPE_NONE 0x0f // absent trap
#define TYPE_GATE 0x8e
#define TYPE_TRAP 0x8f

typedef struct {
   uint16_t offset_low;
   uint16_t selector;
   uint8_t zero;
   uint8_t type_attr;
   uint16_t offset_high;
} idt_descr_t;


static void setup_idtd(idt_descr_t* idtd, uint16_t type, uint32_t handler)
{
	idtd->selector    = KERN_CS;
	idtd->offset_low  = (uint16_t)(handler&0xffff);
	idtd->offset_high = (uint16_t)(handler>>16);
	idtd->zero        = 0;
	idtd->type_attr   = type;
}

static idt_descr_t idt_descrs[MAX_INT_NO];
static interrupt_handler_t interrupt_handlers[MAX_INT_NO];

void interrupts_setup()
{
	int i;

	// setup pic
	pic_init(PIC_BASE);

	// setup irq vectors and 
#define INSTALL_ISR(i) setup_idtd(&idt_descrs[i], TYPE_GATE, (uint32_t)isr##i)
#define INSTALL_NONE(i) setup_idtd(&idt_descrs[i], TYPE_NONE, 0)
	INSTALL_ISR(0);
	INSTALL_ISR(1);
	INSTALL_ISR(2);
	INSTALL_ISR(3);
	INSTALL_ISR(4);
	INSTALL_ISR(5);
	INSTALL_ISR(6);
	INSTALL_ISR(7);
	INSTALL_ISR(8);
	INSTALL_ISR(9);
	INSTALL_ISR(10);
	INSTALL_ISR(11);
	INSTALL_ISR(12);
	INSTALL_ISR(13);
	INSTALL_ISR(14);
	INSTALL_ISR(15);
	INSTALL_ISR(16);
	INSTALL_ISR(17);
	INSTALL_ISR(18);
	INSTALL_ISR(19);
	INSTALL_ISR(20);
	INSTALL_ISR(21);
	INSTALL_ISR(22);
	INSTALL_ISR(23);
	INSTALL_ISR(24);
	INSTALL_ISR(25);
	INSTALL_ISR(26);
	INSTALL_ISR(27);
	INSTALL_ISR(28);
	INSTALL_ISR(29);
	INSTALL_ISR(30);
	INSTALL_ISR(31);
	INSTALL_ISR(32);
	INSTALL_ISR(33);
	INSTALL_ISR(34);
	INSTALL_ISR(35);
	INSTALL_ISR(36);
	INSTALL_ISR(37);
	INSTALL_ISR(38);
	INSTALL_ISR(39);
	INSTALL_ISR(40);
	INSTALL_ISR(41);
	INSTALL_ISR(42);
	INSTALL_ISR(43);
	INSTALL_ISR(44);
	INSTALL_ISR(45);
	INSTALL_ISR(46);
	INSTALL_ISR(47);

	for (i=48; i<128; i++) {
		INSTALL_NONE(i);
	}

	INSTALL_ISR(128);

	// setup idtr
	uint16_t idtr[3];
	idtr[0] = MAX_INT_NO<<3; // MAX_INT_NO entries
	idtr[1] = (uint16_t)((uint32_t)idt_descrs&0xffff);
	idtr[2] = (uint16_t)((uint32_t)idt_descrs>>16);

	// load idt
	__asm__ __volatile__ ( "\tlidt (%0)" ::"g"(idtr));
}




int interrupts_install_handler(int i,interrupt_handler_t isr)
{
	if ((i>=0) && (i<MAX_INT_NO)) {
		interrupt_handlers[i] = isr;
		return 0;
	} else {
		return 1;
	}
}

void isr_handler(registers_t r)
{
	kdebug("irq %d, %d", r.int_no, r.err_code);
	if ((0x20<=r.int_no) && (r.int_no<0x30)) {
		pic_send_eoi(r.int_no-0x20);
	}

   if (interrupt_handlers[r.int_no] != 0)
   {
		interrupt_handlers[r.int_no](&r);
   }
}


