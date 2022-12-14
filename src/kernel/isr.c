// from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#include "kernel/isr.h"
#include "kernel/io.h"
#include "lib/string.h"
#include "lib/log.h"
#include "kernel/pic.h"

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

isr_t interrupt_handlers[256];

void isr_handler(register_t regs)
{
    if (interrupt_handlers[regs.int_no] != 0)
    {
			isr_t handler = interrupt_handlers[regs.int_no];
			handler(&regs);
    }
    else
    {
			printf("[ISR HANDLER] unhandled interrupt: %d, error_code: %d\n", regs.int_no, regs.err_code);
			PANIC("");
    }
}

// This gets called from our ASM interrupt handler stub.
void irq_handler(register_t regs)
{
	// printf("h/w interrupt: %i\n", regs.int_no);
	ack_irq(regs.int_no);

	if (interrupt_handlers[regs.int_no] != 0)
	{
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(&regs);
	}
}

void register_interrupt_handler(uint32_t n, isr_t handler)
{
	if(n < 256) {
		interrupt_handlers[n] = handler;
	}
}