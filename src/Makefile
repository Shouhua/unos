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

QEMU := qemu-system-i386
QEMU_MONITOR_PORT := 45454
QEMU_MEM_CONFIG := -m 32M
QEMU_NETWORK_CONFIG := -netdev tap,id=b1,script=no,downscript=no -device rtl8139,netdev=b1 # 详情可以查看qemu-system-i386 -h
# -netdev tap,id=u1,helper=/usr/lib/qemu/qemu-bridge-helper -device rtl8139,netdev=u1,id=f1 \
# -object filter-dump,id=f1,netdev=u1,file=dump.dat
# -nic tap,mac=52:54:0:12:34:56,model=rtl8139,helper=/usr/lib/qemu/qemu-bridge-helper
# 添加了网络设置, 启动时需要root权限，使用sudo
# QEMU_CONFIG := -cdrom $(BUILD_DIR)/$(OS_ISO) $(QEMU_MEM_CONFIG) $(QEMU_NETWORK_CONFIG)
QEMU_CONFIG := -cdrom $(BUILD_DIR)/$(OS_ISO) $(QEMU_MEM_CONFIG)

# QEMU_GDB_CONFIG := -s -S # 默认使用tcp 1234
# GDB_CLIENT_OPTIONS := target remote localhost:1234

QEMU_GDB_CONFIG := -chardev socket,path=/tmp/gdb-socket,server=on,wait=off,id=gdb0 -gdb chardev:gdb0 -S
GDB_CLIENT_OPTIONS := target remote /tmp/gdb-socket
QEMU_MONITOR_TELNET_CONFIG := -monitor telnet::$(QEMU_MONITOR_PORT),server,nowait &
QEMU_MONITOR_UNIX_SOCKET_CONFIG := -chardev socket,path=/tmp/qemu-monitor-socket,server=on,wait=off,id=qms0 -monitor chardev:qms0 -daemonize
QEMU_DEBUG_CONFIG := $(QEMU_CONFIG) $(QEMU_GDB_CONFIG) $(QEMU_MONITOR_UNIX_SOCKET_CONFIG)

ifeq ($(shell lsb_release -si), Ubuntu)
	CC := i686-elf-gcc
	TERMINAL := gnome-terminal
	TERMINAL_QEMU_TELNET_MONITOR_OPTIONS := -- telnet 127.0.0.1 $(QEMU_MONITOR_PORT)
	TERMINAL_QEMU_US_MONITOR_OPTIONS := -- sudo nc -U /tmp/qemu-monitor-socket
else
	CC := gcc
	TERMINAL := xfce4-terminal --hold
# TERMINAL := xfce4-terminal
	TERMINAL_QEMU_TELNET_MONITOR_OPTIONS := -e "telnet 127.0.0.1 $(QEMU_MONITOR_PORT)"
	TERMINAL_QEMU_US_MONITOR_OPTIONS := -e "sudo nc -U /tmp/qemu-monitor-socket"
endif

CFLAGS := -m32 -fno-stack-protector \
					-ffreestanding \
					-Wall -Wextra -Werror -g -O0
# CFLAGS += -DDEBUG=4

LDFLAGS = -T link.ld -melf_i386

AS = nasm
# 添加-g -F dwarf用于gdb里面调试asm代码(layout asm)
ASFLAGS = -f elf -i boot -g -F dwarf

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
	@./tools/config-grub.sh ${OS_NAME} $(ISO_GRUB_DIR)/grub.cfg
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
	@bochs -q -f tools/bochs.cfg

qemu: all
	@sudo $(QEMU) $(QEMU_CONFIG)

qemu-debug: all
# 网络设备设置必须要使用sudo，这种情况设置gdb server，gdb client也许要使用sudo连接本地的unix socket文件
	@sudo $(QEMU) $(QEMU_DEBUG_CONFIG)
	@sleep 1
	@$(TERMINAL) $(TERMINAL_QEMU_US_MONITOR_OPTIONS)
	@$(TERMINAL) -e 'sudo gdb -s $(BIN_DIR)/$(OS_BIN) -ex "$(GDB_CLIENT_OPTIONS)"'
# ubuntu情況下qemu-system-i386会导致terminal有问题，必须使用reset重置，这里将所有调试界面重新弹出
# ifeq ($(shell lsb_release -si), Ubuntu)
# 	@reset
# endif

qemu-debug-vscode: all
	@$(QEMU) $(QEMU_DEBUG_CONFIG)
	@sleep 0.5
	@$(TERMINAL) $(TERMINAL_QEMU_US_MONITOR_OPTIONS)

clean:
	@rm -rf $(BUILD_DIR)
	@sleep 1
