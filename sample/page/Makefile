BUILD_DIR := build
QEMU_MON_PORT := 55555

all:
	mkdir -p $(BUILD_DIR)
	nasm -f elf -o $(BUILD_DIR)/kernel.o loader.s
	ld -T link.ld -melf_i386 $(BUILD_DIR)/kernel.o -o iso/boot/kernel.bin
	grub-mkrescue -o $(BUILD_DIR)/os.iso iso
	bochs -q

debug:
	@nasm -f elf -g -o $(BUILD_DIR)/kernel.o loader.s
	@ld -T link.ld -melf_i386 $(BUILD_DIR)/kernel.o -o ${BUILD_DIR}/kernel.bin
	@cp ${BUILD_DIR}/kernel.bin iso/boot/
	@grub-mkrescue -o $(BUILD_DIR)/os.iso iso
	@i686-elf-objcopy --only-keep-debug ${BUILD_DIR}/kernel.bin ${BUILD_DIR}/kernel.debug
	@qemu-system-i386 -s -S -cdrom $(BUILD_DIR)/os.iso -monitor telnet::$(QEMU_MON_PORT),server,nowait &
	@sleep 1
	@gnome-terminal -- bash -c "telnet 127.0.0.1 $(QEMU_MON_PORT)"
	@gdb -s $(BUILD_DIR)/kernel.debug -ex "target remote localhost:1234"