// from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#ifndef __ISR_H__
#define __ISR_H__

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

#include "types.h"
typedef struct registers
{
	u32 ds;										// data segment
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
	u32 int_no, err_code;						// interrupt number & error code
	u32 eip, cs, eflags, usereap, ss;			// pushed by processor
} registers_t;

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(registers_t);

void ack_irq(int int_no);

void register_interrupt_handler(u8 n, isr_t handler);

#endif