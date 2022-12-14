// from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#ifndef __ISR_H__
#define __ISR_H__

#include "lib/stdint.h"

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
#define ISR128 128

typedef struct registers
{
	uint32_t ds;										// data segment
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	// interrupt.s中没有err_code会压入0
	uint32_t int_no, err_code;						// interrupt number & error code
	// 有特权级别变化时候，processor会从tss中拿到当前的ss，esp存入，没有变化不会压入ss，esp
	uint32_t eip, cs, eflags, useresp, ss;			// pushed by processor
} register_t;

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(register_t*);

void register_interrupt_handler(uint32_t n, isr_t handler);
extern isr_t interrupt_handlers[256];

#endif