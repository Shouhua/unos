#include "kernel/idt.h"
#include "kernel/io.h"
#include "lib/string.h"
#include "lib/log.h"
#include "kernel/pic.h"

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

extern void idt_flush(idt_ptr_t *);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static inline bool are_interrupts_enabled()
{
	size_t flags;
	asm volatile("pushf\n\t"
				 "pop %0"
				 : "=g"(flags));
	return flags & (1 << 9);
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
		.dpl 			 = IDT_DPL_RING3, // TODO userland是否需要独立的idt
		.zero 		 = 0,
		.d 				 = IDT_GATE_32BIT,
		.gate_type = IDT_GATE_TYPE_INTERRUPT};

	idt_flags_t syscall_flags = {
		.p 				 = IDT_SEGMENT_PRESENT,
		.dpl 			 = IDT_DPL_RING3,
		.zero 		 = 0,
		.d 				 = IDT_GATE_32BIT,
		.gate_type = IDT_GATE_TYPE_INTERRUPT};

	pic_init(0x20, 0x28);

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

	idt_set_gate(128, isr128, IDT_SELECTOR, syscall_flags);

	idt_flush(&idt_ptr);

	// enable hardware interrupts
	asm volatile("sti");
	if (!are_interrupts_enabled())
		// printf("[IDT] Interrupts enabled.\n");
		PANIC("Interrupt are disabed");
}