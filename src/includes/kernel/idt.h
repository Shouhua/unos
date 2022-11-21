#ifndef __IDT_H__
#define __IDT_H__

#include "lib/stdint.h"

struct idt_flags
{
	uint8_t gate_type : 3; // 0 - 2, task(0xf), interrupt(0xe),
	uint8_t d : 1;		  // 3, gate size
	uint8_t zero : 1;	  // 4, value=0
	uint8_t dpl : 2;		  // 5 - 6, descriptor privilege level
	uint8_t p : 1;		  // 7, segment present
} _packed;

typedef struct idt_flags idt_flags_t;

struct idt_entry
{
	uint16_t base_low; // aka offset
	uint16_t segment_selector;
	uint8_t reserved; // always zero
	idt_flags_t flags;
	uint16_t base_high; // aka offset
} _packed;

typedef struct idt_entry idt_entry_t;

struct idt_ptr
{
	uint16_t limit;
	idt_entry_t *base; // aka offset
} _packed;
typedef struct idt_ptr idt_ptr_t;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void isr128();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void init_idt();

void enable_irq(int8_t irq_port);
void disable_irq(int8_t irq_port);

#endif