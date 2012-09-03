#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define MOD_CTRL    0x0100
#define MOD_ALT     0x0200
#define MOD_LSHIFT  0x0400
#define MOD_RSHIFT  0x0800
#define MOD_SHIFT   0x0c00
#define MOD_CAPS    0x1000

int keyboard_init();
void keyboard_interrupt_handler();

uint32_t get_mod_keys();

typedef uint16_t keymap_t[128];
void set_keymap(keymap_t* keymap);

extern keymap_t keyboard_us_map;
extern keymap_t keyboard_fr_map;

#endif
