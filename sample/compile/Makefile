CFLAGS := -m32
# Darwin不支持intel
# CFLAGS += -masm=intel
CFLAGS += -Qn
# CFLAGS += -fno-builtin
# CFLAGS += -nostdinc
# PIC(position independent code), 用于获取符号位置相对位置
# 比如call __x86.get_pc_thunk.ax
CFLAGS += -fno-pic 
# CFLAGS += -fno-pie # position independent executable
# CFLAGS += -nostdlib
CFLAGS += -fomit-frame-pointer
# CFLAGS += -fno-stack-protector # push ebp ...
CFLAGS += -mpreferred-stack-boundary=2 # 不需要栈对齐
# CFI(call frame information) 调用栈帧信息，用于调试获取调用异常
CFLAGS += -fno-asynchronous-unwind-tables 

CFLAGS := $(strip ${CFLAGS})

.PHONY: hello.s
hello.s: hello.c
	gcc $(CFLAGS) -S $< -o $@