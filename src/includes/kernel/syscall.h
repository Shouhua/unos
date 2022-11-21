#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "kernel/isr.h"

#define NUM_SYSCALLS 5
extern void * syscall_table[NUM_SYSCALLS];
void syscall_dispatcher(register_t * regs);
void syscall_init();
void _exit();

#endif
