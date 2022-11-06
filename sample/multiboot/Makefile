KERNEL_DIR := kernel
BUILD_DIR := build
INCLUDES_DIR := includes
OBJECT_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
ISO_DIR := $(BUILD_DIR)/iso
ISO_BOOT_DIR := $(ISO_DIR)/boot
ISO_GRUB_DIR := $(ISO_BOOT_DIR)/grub
OS_NAME := unos
OS_BIN = $(OS_NAME).bin
OS_ISO = $(OS_NAME).iso

QEMU_MON_PORT := 55555

CC := i686-elf-gcc
CFLAGS := -m32 -fno-stack-protector \
					-ffreestanding \
					-Wall -Wextra -Werror -g
LDFLAGS = -T link.ld -melf_i386

AS = nasm
ASFLAGS = -f elf

INCLUDES := $(patsubst %, -I%, $(INCLUDES_DIR))
C_SOURCE_FILES := $(shell find -name "*.[c]")
C_SRC := $(patsubst ./%, $(OBJECT_DIR)/%.o, $(C_SOURCE_FILES))
ASM_SOURCE_FILES := $(shell find -name "*.[s]")
ASM_SRC := $(patsubst ./%, $(OBJECT_DIR)/%.o, $(ASM_SOURCE_FILES))

$(OBJECT_DIR):
	@mkdir -p $(OBJECT_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(ISO_DIR):
	@mkdir -p $(ISO_DIR)
	@mkdir -p $(ISO_BOOT_DIR)
	@mkdir -p $(ISO_GRUB_DIR)

$(BIN_DIR)/$(OS_BIN): $(OBJECT_DIR) $(BIN_DIR) $(C_SRC) $(ASM_SRC)
	@echo " LINK: $(BIN_DIR)/$(OS_BIN)"
	@ld $(LDFLAGS) $(C_SRC) $(ASM_SRC) -o $@

$(BUILD_DIR)/$(OS_ISO): $(ISO_DIR) $(BIN_DIR)/$(OS_BIN)
	@./config-grub.sh ${OS_NAME} $(ISO_GRUB_DIR)/grub.cfg
	@cp $(BIN_DIR)/$(OS_BIN) $(ISO_BOOT_DIR)
	@grub-mkrescue -o $(BUILD_DIR)/$(OS_ISO) $(ISO_DIR)

$(OBJECT_DIR)/%.c.o : %.c
	@mkdir -p $(@D)
	@echo " BUILD: $<"
	@$(CC) $(INCLUDES) -c $< -o $@ $(CFLAGS)

$(OBJECT_DIR)/%.s.o : %.s
	@mkdir -p $(@D)
	@echo " BUILD: $<"
	@$(AS) $(ASFLAGS) $< -o $@

.PHONY: all bochs clean
all: clean $(BUILD_DIR)/$(OS_ISO)

bochs: all
	@bochs -q -f bochs.cfg

qemu: all
	qemu-system-i386 -cdrom $(BUILD_DIR)/$(OS_ISO) -m 32M

qemu-debug: all
	@qemu-system-i386 -s -S -cdrom $(BUILD_DIR)/$(OS_ISO) -monitor telnet::$(QEMU_MON_PORT),server,nowait &
	@sleep 1
	@gnome-terminal -- bash -c "telnet 127.0.0.1 $(QEMU_MON_PORT)"
	@gdb -s $(BIN_DIR)/$(OS_BIN) -ex "target remote localhost:1234"

clean:
	@rm -rf $(BUILD_DIR)
	@sleep 1