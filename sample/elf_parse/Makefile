# := 解析时间立即展开；= 使用时展开
# TODO hello-pie.asm链接时需要使用-pie，但是其他asm文件不需要，该怎么写
# 想法1: ifeq($(<F), $(findstring $(<F), $(ASM_PIE_SRC)))**不行**。
# 因为ifeq里面参数是读取时解析，此时$(<F)为空字符串
# 想法2: https://stackoverflow.com/questions/9355162/makefile-with-different-compiler-flags-for-source-files
# 不成功，想为ASM_PIE_SRC设置规则，$(ASM_PIE_OBJ): LDFLAGS := -pie 但是不成功
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

ASM_SRC := $(shell find . -name "*.asm")
ASM_PIE_SRC := ./hello-pie.asm
ASM_NO_PIE_SRC := $(filter-out $(ASM_PIE_SRC), $(ASM_SRC))
ASM_PIE_OBJ := $(patsubst %.o, $(OBJ_DIR)/%.o, $(ASM_PIE_SRC))
ASM_NO_PIE_OBJ := $(patsubst %.o, $(OBJ_DIR)/%.o, $(ASM_NO_PIE_SRC))
ASM_OBJ := $(patsubst ./%.asm, $(OBJ_DIR)/%.o, $(ASM_SRC))
ASM_BIN := $(patsubst ./%.asm, $(BIN_DIR)/%, $(ASM_SRC))

C_SRC := $(shell find . -name "*.c")
C_OBJ := $(patsubst ./%.c, $(OBJ_DIR)/%.o, $(C_SRC))
C_BIN := $(patsubst ./%.c, $(BIN_DIR)/%, $(C_SRC))
CFLAGS := -g -o

.PHONY: all clean
all: build_dir $(ASM_BIN) $(C_BIN)

$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	ld $< -o $@
$(OBJ_DIR)/%.o: %.asm
	nasm -f elf64 $< -o $@

$(BIN_DIR)/%: %.c
	gcc $< $(CFLAGS) $@

build_dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
run_elk: $(BIN_DIR)/elk $(BIN_DIR)/hello
	$(BIN_DIR)/elk $(BIN_DIR)/hello

clean:
	rm -rf $(BUILD_DIR)