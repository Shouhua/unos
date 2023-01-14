# := 解析时间立即展开；= 使用时展开
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

ASM_SRC := $(shell find . -name '*.asm')
ASM_PIE := hello-pie
ASM_OBJ := $(patsubst ./%.asm, $(OBJ_DIR)/%.o, $(ASM_SRC))
ASM_BIN := $(patsubst ./%.asm, $(BIN_DIR)/%, $(ASM_SRC))

C_SRC := $(shell find . -name '*.c')
C_OBJ := $(patsubst ./%.c, $(OBJ_DIR)/%.o, $(C_SRC))
C_BIN := $(patsubst ./%.c, $(BIN_DIR)/%, $(C_SRC))
CFLAGS := -g -o

.PHONY: clean all
all: $(ASM_BIN)

$(ASM_BIN): $(ASM_OBJ) build_dir
ifneq ($(patsubst $(OBJ_DIR)/%.o,%, $<),$(findstring $(patsubst $(OBJ_DIR)/%.o, %, $<), $(ASM_PIE)))
	ld $< -o $@
else
	ld -pie $< -o $@
endif
$(ASM_OBJ): $(ASM_SRC) build_dir
	nasm -f elf64 $< -o $@
# $(BIN_DIR)/%: %.asm build_dir
# 	nasm -f elf64 $< -o $(OBJ_DIR)/$(patsubst %.asm, %, $<)
# 	ld $(OBJ_DIR)/$(patsubst %.asm, %, $<) -o $@

$(BIN_DIR)/%: %.c build_dir
	gcc $< $(CFLAGS) $@

build_dir:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)
