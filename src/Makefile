ARCH=i386

CROSS=/home/ben/prog/myos/cross/bin/i586-elf
AS=$(CROSS)-as
CC=$(CROSS)-gcc
LD=$(CROSS)-ld
DS=$(CROSS)-objdump

CFLAGS=-Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -Iinclude -I. -Iarch/$(ARCH) -g

ARCH_SRC  = boot/boot.c kernel/util.c kernel/spinlock.c 
ARCH_SRC += kernel/gdt.c kernel/paging.c
ARCH_SRC += kernel/interrupts.c kernel/isr_wrappers.c kernel/pic.c kernel/timer.c kernel/syscall.c
ARCH_SRC += kernel/arch-task.c 
ARCH_SRC += kernel/vgatext.c 
ARCH_SRC += kernel/kernel.c

COMMON_SRC += kernel/kmalloc.c
COMMON_SRC += kernel/task.c kernel/scheduler.c
COMMON_SRC += kernel/mutex.c kernel/semaphore.c

COMMON_SRC += drivers/keyboard.c

OBJ = $(ARCH_SRC:%.c=arch/$(ARCH)/%.o) $(COMMON_SRC:%.c=%.o)



kernel.elf: linker.ld $(OBJ)
	$(LD) -T linker.ld -o $@ $(OBJ)

a.img: kernel.elf
	dd if=/dev/zero of=pad bs=1 count=750
	cat ../grub/stage1 ../grub/stage2 pad $< > $@
	rm pad

floppy.img: kernel.elf
	sudo losetup /dev/loop0 floppy.img
	sudo mount /dev/loop0 /mnt
	sudo cp kernel.elf /mnt/kernel
	sudo umount /dev/loop0
	sudo losetup -d /dev/loop0 

c.img: kernel.elf
	dd if=/dev/zero of=$@ bs=4k count=1024 2>/dev/null # 4MB
	mkfs.vfat $@ || fail "could not format disk image"
	syslinux $@ || fail "could not install syslinux"
	mcopy -i $@ /usr/lib/syslinux/mboot.c32 ::mboot.c32 || fail "could not copy over mboot.c32"
	mcopy -i $@ $< ::kernel.elf || fail "could not copy over kernel"

clean:
	rm -f $(OBJ) kernel.elf a.img c.img



%.o: %.S
	$(AS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<





 DSFLAGS = -C -S -EL

%.dis: %.elf
	$(DS) $(DSFLAGS) $^ > $@
