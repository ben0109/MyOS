#include <stdint-gcc.h>
#include "kernel/klog.h"
#include "kernel/util.h"
#include "kernel/vgatext.h"
#include "kernel/interrupts.h"
#include "kernel/pic.h"
#include "drivers/keyboard.h"

#define BUFFER_LENGTH 8

#define KBD_STATUS_REG  0x64 /* Status register (R) */
#define KBD_CONTROL_REG 0x64 /* Controller command register (W) */
#define KBD_DATA_REG    0x60 /* Keyboard data register (R/W) */

#define KBD_STATUS_OBS           0x01
#define KBD_STATUS_IBS           0x02
#define KBD_STATUS_SYSF          0x04
#define KBD_STATUS_COMMAND_DATA  0x08
#define KBD_STATUS_TIMEOUT_ERROR 0x40
#define KBD_STATUS_PARITY_ERROR  0x80

#define read_status()    (io_inb (KBD_STATUS_REG))
#define write_control(b) (io_outb(KBD_CONTROL_REG,b))
#define read_data()      (io_inb (KBD_DATA_REG))
#define write_data(b)    (io_outb(KBD_DATA_REG,b))
/*
enum {
	INIT,
	IDLE,
	WAIT_ACK
} state;

uint8_t buffer[BUFFER_LENGTH];
int buffer_pos = 0;

void send_command(uint8_t command)
{
	while (read_status()&KBD_STATUS_IBS) ;
	write_control(command);
	io_wait();
}

void send_data(uint8_t byte)
{
	while (read_status()&KBD_STATUS_IBS) ;
	write_data(byte);
	io_wait();
}

uint8_t wait_and_read_data()
{
	while ((read_status()&KBD_STATUS_OBS)==0) ;
	return read_data();
}

int wait_ack()
{
	return wait_and_read_data()==0xfa;
}


int keyboard_init()
{
	uint8_t byte;

	send_command(0xad); // disable port 1
	send_command(0xa7); // disable port 2
	read_data();              // flush output buffer

	// set config
	send_command(0x20);
	byte = wait_and_read_data();
	byte &= ~0x23;
	send_command(0x60);
	send_command(byte);

	// sefl test
	send_command(0xaa);
	byte = wait_and_read_data();
	if (byte!=0x55) {
		return -1;
	}

	// test port 1
	send_command(0xab);
	byte = wait_and_read_data();
	if (byte!=0x00) {
		return -1;
	}

	// enable port 1
	send_command(0xae);

	// reset device 1
	send_data(0xff);
	byte = wait_and_read_data();
	if (byte!=0x00) {
		return -1;
	}

	return 0;
}


void handle_keyboard_event(uint8_t scancode)
{
	if (scancode == 0xfa) {  // ACK
//		acknowledge = 1;
		kdebug("ACK");
	} else if (scancode == 0xfe) { // RESEND
//		resend = 1;
		kdebug("RESEND");
	} else {
//		int up = scancode&0x80;
//		char key = scancode&0x7f;
		kdebug("scancode: %b",scancode);
	}
}


void keyboard_interrupt()
{
	uint8_t status;
	status = read_status();

	while (status&KBD_STATUS_OBS) {
		uint8_t byte = read_data();
		handle_keyboard_event(byte);
		status = read_status();
	}
}
*/

uint16_t keyboard_us_map[128] =
{
	0,  
	27, /* esc */
	'1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=',
	'\b', /* Backspace */
	'\t', /* tab */
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'\n', /* Enter key */
	MOD_CTRL, 
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	MOD_LSHIFT, 
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',	'm', ',', '.', '/', 
	MOD_RSHIFT,
	'*',
	MOD_ALT,	/* Alt */
	' ',	/* Space bar */
	MOD_CAPS,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
0,	/* All other keys are undefined */
};

uint16_t keyboard_fr_map[128] =
{
	0,  
	27, /* esc */
	'1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', ')', '=',
	'\b', /* Backspace */
	'\t', /* tab */
	'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$',
	'\n', /* Enter key */
	MOD_CTRL, 
	'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '`', '2',
	MOD_LSHIFT, 
	'*', 'w', 'x', 'c', 'v', 'b', 'n',	',', ';', ':', '!', 
	MOD_RSHIFT,
	'<',
	MOD_ALT,	/* Alt */
	' ',	/* Space bar */
	MOD_CAPS,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   '<',
	0,	/* F11 Key */
	0,	/* F12 Key */
0,	/* All other keys are undefined */
};

int keyboard_init()
{
	interrupts_install_handler(PIC_IRQ(1), keyboard_interrupt_handler);
	pic_enable_line(1);
	return 0;
}

void keyboard_interrupt_handler()
{
	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = io_inb(0x60);

	if (scancode & 0x80) {
		/* You can use this one to see if the user released the
		*  shift, alt, or control keys... */
	} else {
		kdebug("scancode=%b", scancode);
		uint16_t code = keyboard_fr_map[scancode];
		if (code&~0xff) {
			// mod key
		} else {
			vgatext_putchar(code);
		}
	}
}
