#include <stdint-gcc.h>
#include "kernel/util.h"

int console_x = 0;
int console_y = 0;

void move_cursor(void)
{
    unsigned temp;

    temp = console_y * 80 + console_x;

    io_outb(0x3D4, 14);
    io_outb(0x3D5, temp >> 8);
    io_outb(0x3D4, 15);
    io_outb(0x3D5, temp);
}


void vgatext_clear()
{
	uint16_t* ptr = (uint16_t*)0xB8000;
	int i;
	for (i=0; i<25*80; i++) {
		*ptr++ = 0x0720; /* space, light grey (7) on black (0) */
	}
	console_x = 0;
	console_y = 0;
	move_cursor();
}

void vgatext_new_line()
{
	console_x = 0;
	if (++console_y>24) {

		uint16_t* video_memory = (uint16_t*)0xb8000;
		int i;

		console_y = 24;

		// scroll one line up
		for (i=0; i<24*80; i++) {
			video_memory[i] = video_memory[i+80];
		}

		// clear last line
		for (i=24*80; i<25*80; i++) {
			video_memory[i] = 0;
		}
	}
	move_cursor();
}

void vgatext_putchar(char c)
{
	if (c=='\n') {
		vgatext_new_line();

	} else {
		uint8_t byte = (uint8_t)c;
		uint16_t* ptr = (uint16_t*)(0xb8000 + 2*(console_y*80 + console_x));
		*ptr = 0x0700 | byte;

		if (++console_x==80) {
			vgatext_new_line();
			console_x = 0;
		}
	}
	move_cursor();
}

void vgatext_puts(const char* text)
{
	while (*text) {
		vgatext_putchar(*text++);
	}			
}


#include "kernel/interrupts.h"
#include "kernel/vgatext.h"
#include <stdarg.h>

const char hex[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

const char *level_names[] = { "panic", "error", "warn ", "info ", "debug" };

void kprintf(int level, const char *fmt,...)
{
	va_list pa;
	va_start(pa, fmt);

	vgatext_putchar('[');
	vgatext_puts(level_names[level]);
	vgatext_putchar(']');
	vgatext_putchar(' ');

	while (*fmt) {
		char c = *fmt++;
		if (c=='%') {
			uint8_t  b;
			uint16_t w;
			uint32_t d;
			char *s;
			switch (*fmt++) {
			case '%':
				vgatext_putchar('%');
				break;
			case 'b': /* affichage d'un caractère */ 
				b = (uint8_t)va_arg(pa, int);
				vgatext_putchar(hex[(b>> 4)&15]);
				vgatext_putchar(hex[(b>> 0)&15]);
				break;
			case 'w': /* affichage d'un entier */ 
				w = (uint16_t)va_arg(pa, int);
				vgatext_putchar(hex[(w>>12)&15]);
				vgatext_putchar(hex[(w>> 8)&15]);
				vgatext_putchar(hex[(w>> 4)&15]);
				vgatext_putchar(hex[(w>> 0)&15]);
				break;
			case 'd':
				d = va_arg(pa, uint32_t);
				vgatext_putchar(hex[(d>>28)&15]);
				vgatext_putchar(hex[(d>>24)&15]);
				vgatext_putchar(hex[(d>>20)&15]);
				vgatext_putchar(hex[(d>>16)&15]);
				vgatext_putchar(hex[(d>>12)&15]);
				vgatext_putchar(hex[(d>> 8)&15]);
				vgatext_putchar(hex[(d>> 4)&15]);
				vgatext_putchar(hex[(d>> 0)&15]);
				break;
			case 's':
				s = va_arg(pa, char*);
				vgatext_puts(s);
				break;
			default:
				/* silently ignore */
				break;
			}

		} else {
			vgatext_putchar(c);
		}
	}   

	vgatext_putchar('\n');

	// halt the kernel if panic
	if (level==0) {
		cli();
		while(1);
	}

	va_end(pa);
}

