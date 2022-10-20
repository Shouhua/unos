// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

#ifndef __DESCRIPTOR_TABLES_H__
#define __DESCRIPTOR_TABLES_H__

#include "types.h"

// ------------------------
// Global Descriptor Tables
// ------------------------

struct gdt_access
{
	u8 type : 4; // 36 - 39: segment type - code segment
	u8 dt : 1;	 // 40: descriptor type
	u8 dpl : 2;	 // 41 - 42: descriptor privilege level - Ring 0 - 3
	u8 p : 1;	 // 43: is segment present? (1=yes)
} _packed;
typedef struct gdt_access gdt_access_t;

struct gdt_entry
{
	u16 limit_low : 16;	 // 0 - 15
	u32 base_low : 24;	 // 16 - 35
	gdt_access_t access; // 36 - 43
	u8 limit_high : 4;	 // 44 - 46
	u8 a : 1;			 // 47: available for system use, must be 0
	u8 unused : 1;		 // 48: must be 0
	u8 d : 1;			 // 49: operand size (0 = 16 bit, 1 = 32 bit)
	u8 g : 1;			 // 50: granularity (0 = 1 Byte, 1 = 1KiB)
	u8 base_high : 8;	 // 51 - 58
} _packed;
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr
{
	u16 limit;
	gdt_entry_t *base;
} _packed;
typedef struct gdt_ptr gdt_ptr_t;

void init_descriptor_tables();

// ---------------------------
// Interrupt Descriptor Tables
// ---------------------------

struct idt_flags
{
	u8 gate_type : 3; // 0 - 2, task, interrupt,
	u8 d : 1;		  // 3, gate size
	u8 zero : 1;	  // 4, value=0
	u8 dpl : 2;		  // 5 - 6, descriptor privilege level
	u8 p : 1;		  // 7, segment present
} _packed;
typedef struct idt_flags idt_flags_t;

struct idt_entry
{
	u16 base_low; // aka offset
	u16 segment_selector;
	u8 reserved; // always zero
	idt_flags_t flags;
	u16 base_high; // aka offset
} _packed;
typedef struct idt_entry idt_entry_t;

struct idt_ptr
{
	u16 limit;
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

#endif