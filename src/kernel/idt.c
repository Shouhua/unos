#include "kernel/idt.h"
#include "kernel/io.h"
#include "lib/string.h"
#include "lib/log.h"

#define IDT_SELECTOR 0x08

// Task type (3 bits)
#define IDT_GATE_TYPE_TASK 0x5
#define IDT_GATE_TYPE_INTERRUPT 0x6
#define IDT_GATE_TYPE_TRAP 0x7

// D (1 bit)
#define IDT_GATE_16BIT 0
#define IDT_GATE_32BIT 1

// Present (1 bit)
#define IDT_SEGMENT_NOT_PRESENT 0
#define IDT_SEGMENT_PRESENT 1

// DPL Field (2 bits)
#define IDT_DPL_RING0 0
#define IDT_DPL_RING1 1
#define IDT_DPL_RING2 2
#define IDT_DPL_RING3 3

// PIC - from http://wiki.osdev.org/8259_PIC
#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

// 0x20, 0xA0
#define ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE 0x02	/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4bytes (8bytes) */
#define ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define ICW1_INIT 0x10		/* Initialization - required! */

#define ICW4_8086 0x01		 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08	 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10		 /* Special fully nested (not) */

extern void idt_flush(idt_ptr_t *);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static inline void io_wait()
{
	asm ("NOP");
}

static inline bool are_interrupts_enabled()
{
	size_t flags;
	asm volatile("pushf\n\t"
				 "pop %0"
				 : "=g"(flags));
	return flags & (1 << 9);
}

// from http://wiki.osdev.org/8259_PIC
// master 8059A 0x20-0x27, slave 8059A 0x28-0x2f
static void PIC_remap(uint8_t offset1, uint8_t offset2)
{
	// uint8_t a1, a2;

	// a1 = inb(PIC1_DATA); // save masks
	// a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4); // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// outb(PIC1_DATA, a1); // restore saved masks.
	// outb(PIC2_DATA, a2);
	// 必须要使用0，使用上面保存的masks，rtl8139不能收到中断
	outb(PIC1_DATA, 0); // restore saved masks.
	outb(PIC2_DATA, 0);
}

static void idt_set_gate(
	uint8_t idx,
	void(*base),
	uint16_t selector,
	idt_flags_t flags)
{

	idt_entries[idx] = (struct idt_entry){
		.base_low 				= (uint32_t)base & 0xffff,
		.base_high 				= ((uint32_t)base >> 16) & 0xffff,
		.segment_selector = selector,
		.flags 						= flags
	};
}

void init_idt()
{
	// printf("[IDT] Begin init idt\n");

	// printf("cpuHasMSR=%i\n", cpuHasMSR());
	// printf("Disabling APIC...\n");
	// uint32_t msr = disable_apic();
	// printf("MSR 0x1b=%x\n", msr);

	idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base = idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

	idt_flags_t flags = {
		.p 				 = IDT_SEGMENT_PRESENT,
		.dpl 			 = IDT_DPL_RING0,
		.zero 		 = 0,
		.d 				 = IDT_GATE_32BIT,
		.gate_type = IDT_GATE_TYPE_INTERRUPT};

	idt_set_gate(0, isr0, IDT_SELECTOR, flags);
	idt_set_gate(1, isr1, IDT_SELECTOR, flags);
	idt_set_gate(2, isr2, IDT_SELECTOR, flags);
	idt_set_gate(3, isr3, IDT_SELECTOR, flags);
	idt_set_gate(4, isr4, IDT_SELECTOR, flags);
	idt_set_gate(5, isr5, IDT_SELECTOR, flags);
	idt_set_gate(6, isr6, IDT_SELECTOR, flags);
	idt_set_gate(7, isr7, IDT_SELECTOR, flags);
	idt_set_gate(8, isr8, IDT_SELECTOR, flags);
	idt_set_gate(9, isr9, IDT_SELECTOR, flags);
	idt_set_gate(10, isr10, IDT_SELECTOR, flags);
	idt_set_gate(11, isr11, IDT_SELECTOR, flags);
	idt_set_gate(12, isr12, IDT_SELECTOR, flags);
	idt_set_gate(13, isr13, IDT_SELECTOR, flags);
	idt_set_gate(14, isr14, IDT_SELECTOR, flags);
	idt_set_gate(15, isr15, IDT_SELECTOR, flags);
	idt_set_gate(16, isr16, IDT_SELECTOR, flags);
	idt_set_gate(17, isr17, IDT_SELECTOR, flags);
	idt_set_gate(18, isr18, IDT_SELECTOR, flags);
	idt_set_gate(19, isr19, IDT_SELECTOR, flags);
	idt_set_gate(20, isr20, IDT_SELECTOR, flags);
	idt_set_gate(21, isr21, IDT_SELECTOR, flags);
	idt_set_gate(22, isr22, IDT_SELECTOR, flags);
	idt_set_gate(23, isr23, IDT_SELECTOR, flags);
	idt_set_gate(24, isr24, IDT_SELECTOR, flags);
	idt_set_gate(25, isr25, IDT_SELECTOR, flags);
	idt_set_gate(26, isr26, IDT_SELECTOR, flags);
	idt_set_gate(27, isr27, IDT_SELECTOR, flags);
	idt_set_gate(28, isr28, IDT_SELECTOR, flags);
	idt_set_gate(29, isr29, IDT_SELECTOR, flags);
	idt_set_gate(30, isr30, IDT_SELECTOR, flags);
	idt_set_gate(31, isr31, IDT_SELECTOR, flags);

	idt_set_gate(128, isr128, IDT_SELECTOR, flags);

	// Remap the irq table
	// printf("[IDT] Remapping IRQs\n");
	PIC_remap(0x20, 0x28);
	// ICW1(Initialization Command Word)
	// outb(0x20, 0x11);
	// outb(0xA0, 0x11);
	// ICW2 set remap	
	// outb(0x21, 0x20);
	// outb(0xA1, 0x28);
	// ICW3 master IR3 cascade slave -> slave irq10(2)
	// outb(0x21, 0x04);
	// outb(0xA1, 0x02);
	// ICW4 (80x86 mode)
	// outb(0x21, 0x01);
	// outb(0xA1, 0x01);
	// OCW(operation control word) 开启所有终端
	// outb(0x21, 0x0);
	// outb(0xA1, 0x0);

	idt_set_gate(32, irq0, IDT_SELECTOR, flags);
	idt_set_gate(33, irq1, IDT_SELECTOR, flags);
	idt_set_gate(34, irq2, IDT_SELECTOR, flags);
	idt_set_gate(35, irq3, IDT_SELECTOR, flags);
	idt_set_gate(36, irq4, IDT_SELECTOR, flags);
	idt_set_gate(37, irq5, IDT_SELECTOR, flags);
	idt_set_gate(38, irq6, IDT_SELECTOR, flags);
	idt_set_gate(39, irq7, IDT_SELECTOR, flags);
	idt_set_gate(40, irq8, IDT_SELECTOR, flags);
	idt_set_gate(41, irq9, IDT_SELECTOR, flags);
	idt_set_gate(42, irq10, IDT_SELECTOR, flags);
	idt_set_gate(43, irq11, IDT_SELECTOR, flags);
	idt_set_gate(44, irq12, IDT_SELECTOR, flags);
	idt_set_gate(45, irq13, IDT_SELECTOR, flags);
	idt_set_gate(46, irq14, IDT_SELECTOR, flags);
	idt_set_gate(47, irq15, IDT_SELECTOR, flags);

	idt_flush(&idt_ptr);

	// enable hardware interrupts
	asm volatile("sti");
	if (!are_interrupts_enabled())
		// printf("[IDT] Interrupts enabled.\n");
		PANIC("Interrupt are disabed");
}

void enable_irq(int8_t irq_port) {
	uint8_t port;
	uint8_t value;

	if(irq_port < 8) {
			port = PIC1_DATA;
	} else {
			port = PIC2_DATA;
			irq_port -= 8;
	}
	value = inb(port) & ~(1 << irq_port);
	outb(port, value);  
}

void disable_irq(int8_t irq_port) {
	uint8_t port;
	uint8_t value;

	if(irq_port < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_port -= 8;
	}
	value = inb(port) | (1 << irq_port);
	outb(port, value);
}