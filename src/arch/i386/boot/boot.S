.global loader                          # making entry point visible to linker

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0                     # align loaded modules on page boundaries
.set MEMINFO,  1<<1                     # provide memory map
.set FLAGS,    ALIGN | MEMINFO          # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002               # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)         # checksum required

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# reserve initial kernel stack space
.set STACKSIZE, 0x4000                  # that is, 16k.
.lcomm stack, STACKSIZE                 # reserve 16k stack on a doubleword boundary
.comm  mbd, 4                           # we will use this in kmain
.comm  magic, 4                         # we will use this in kmain

loader:
    movl  %eax, magic                   # Multiboot magic number
    movl  %ebx, mbd                     # Multiboot data structure

	lgdt  gdt_48                        # load gdt

	movl  $0x18, %eax                   # setup data segments
	movl  %eax, %ds
	movl  %eax, %es
	movl  %eax, %fs
	movl  %eax, %gs
	movl  %eax, %ss

    movl  $(stack + STACKSIZE), %esp    # set up the stack

    lcall  far_call_to_kmain            # far call to kmain (set cs as well)

    cli
hang:
    #hlt                                 # halt machine should kernel return
    jmp   hang




far_call_to_kmain:
	.long kmain
	.word 0x10

gdt_48:
	.word 0x0030                         # 6 entries
	.long gdt_entries

.align 4
.globl gdt_entries
gdt_entries:
	.word	0, 0, 0, 0                   # invalid => using seg 0 raises a PF
	.word	0, 0, 0, 0                   # task state segment (filled in later)
	.word	0xFFFF,0,0x9a00,0x00cf       # kernel, 4Gb starting @ 0, code read/exec
	.word	0xFFFF,0,0x9200,0x00cf       # kernel, 4Gb starting @ 0, data read/write
	.word	0xFFFF,0,0xfa00,0x00cf       # user, 4Gb starting @ 0, code read/exec
	.word	0xFFFF,0,0xf200,0x00cf       # user, 4Gb starting @ 0, data read/write

