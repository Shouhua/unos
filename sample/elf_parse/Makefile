# := 解析时间立即展开；= 使用时展开
# TODO hello-pie.asm链接时需要使用-pie，但是其他asm文件不需要，该怎么写
# 想法1: ifeq($(<F), $(findstring $(<F), $(ASM_PIE_SRC)))**不行**。
# 因为ifeq里面参数是读取时解析，此时$(<F)为空字符串
# 想法2: https://stackoverflow.com/questions/9355162/makefile-with-different-compiler-flags-for-source-files
# 不成功，想为ASM_PIE_SRC设置规则，$(ASM_PIE_OBJ): LDFLAGS := -pie 但是不成功
# 2023-02-02解决 \
Makefile checklist: https://gist.github.com/rueycheng/42e355d1480fd7a33ee81c866c7fdf78 \
https://www.gnu.org/software/make/manual/html_node/Static-Usage.html \
1. targets: targets-pattern: prereq-patterns\
	recipes\
2. 多个重复的target，每个都执行，但是会使用最后一个结果; 当执行pie时设置ASM_FLAGS, 可以做些前期准备工作
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# 使用-pie链接的asm文件
#ASM_SRC := $(shell find . -type f -name '*.asm' -exec basename {} \;) # 使用find需要去除文件前缀，比如./
ASM_SRC := $(wildcard *.asm) # wildcard展开占位符

# 去除生成使用lib的asm文件
ASM_LIB := msg.asm hello-dl.asm
ASM_SRC := $(filter-out $(ASM_LIB), $(ASM_SRC))

ASM_PIE_SRC := hello-pie.asm hello-mov-pie.asm # 可以使用文件名是否包含pie字符提取文件
ASM_PIE_OBJ := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(ASM_PIE_SRC))
ASM_PIE_BIN := $(patsubst %.asm, $(BIN_DIR)/%, $(ASM_PIE_SRC))

# 不使用-pie链接的asm文件
ASM_NO_PIE_SRC := $(filter-out $(ASM_PIE_SRC), $(ASM_SRC))
ASM_NO_PIE_OBJ := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(ASM_NO_PIE_SRC))

# 所有asm文件不包括生成so文件的asm
ASM_OBJ := $(patsubst %.asm, $(OBJ_DIR)/%.o, $(ASM_SRC))
ASM_BIN := $(patsubst %.asm, $(BIN_DIR)/%, $(ASM_SRC))

# c文件
C_SRC := $(wildcard *.c)
C_OBJ := $(patsubst %.c, $(OBJ_DIR)/%.o, $(C_SRC))
C_BIN := $(patsubst %.c, $(BIN_DIR)/%, $(C_SRC))
CFLAGS := -g -o

.PHONY: all clean
all: build_dir $(ASM_BIN) $(C_BIN) $(BIN_DIR)/hello-dl

$(BIN_DIR)/hello-dl: $(OBJ_DIR)/hello-dl.o $(OBJ_DIR)/msg.o
	ld -pie --dynamic-linker /lib/ld-linux-x86-64.so.2 $^ -o $@

$(ASM_BIN): $(BIN_DIR)/%: $(OBJ_DIR)/%.o
	@ld $(ASM_FLAGS) $< -o $@

$(ASM_PIE_BIN): ASM_FLAGS := -pie
$(OBJ_DIR)/%.o: %.asm
	@nasm -f elf64 -gdwarf $< -o $@

$(BIN_DIR)/%: %.c
	@gcc $< $(CFLAGS) $@

build_dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

run_elk: $(BIN_DIR)/elk $(BIN_DIR)/hello
	$(BIN_DIR)/elk $(BIN_DIR)/hello

debug: $(BIN_DIR)/elk
	gdb $(BIN_DIR)/elk

clean:
	rm -rf $(BUILD_DIR)
