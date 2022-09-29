OBJECTS = multiboot2.o resetblink.o framebuffer.o loader.o kernel.o \
					io.o serial.o string.o \
					# interrupt.o descriptor_tables.o dt.o isr.o timer.o apic.o
CC = gcc
# CFLAGS = -m32 -nostdlib -fno-builtin -fno-stack-protector \
# 		 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
CFLAGS = -m32 -fno-stack-protector \
					-ffreestanding \
					-Wall -Wextra -Werror -g -c
# things you may or may not need -nostdinc -fno-builtin -nostdlib -nostartfiles -nodefaultlibs
# see http://forum.osdev.org/viewtopic.php?f=1&t=25585
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: unos.iso

unos.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	grub-file --is-x86-multiboot2 kernel.elf
	# 0-ok, 1-fail
	echo 'kernel.elf is whether multiboot2: $?'
	grub-mkrescue -o unos.iso iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

# unos.iso: kernel.elf
# 	cp kernel.elf iso/boot/kernel.elf
# 	genisoimage -R                              \
#                     -b boot/grub/stage2_eltorito    \
#                     -no-emul-boot                   \
#                     -boot-load-size 4               \
#                     -A os                           \
#                     -input-charset utf8             \
#                     -quiet                          \
#                     -boot-info-table                \
#                     -o unos.iso                       \
#                     iso

run: unos.iso
	bochs -f bochsrc -q

%.o : %.c
	$(CC) ${CFLAGS} $< -o $@

%.o : %.s
	$(AS) ${ASFLAGS} $< -o $@

dump: kernel.elf
	readelf -WS kernel.elf
	objdump -D -Mintel-mnemonics kernel.elf

clean:
	rm -rf *.o kernel.elf os.iso iso/boot/kernel.elf kernel.iso unos.iso
