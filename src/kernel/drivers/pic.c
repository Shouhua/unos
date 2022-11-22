#include "kernel/pic.h"
#include "kernel/io.h"

static inline void io_wait()
{
	asm ("NOP");
}

/*
 * PIC is very complex, for a better understanding, visist
 * http://www.brokenthorn.com/Resources/OSDevPic.html or some other materials that explain PIC, otherwise the following code is impossible to uderstand....
 * master 8059A 0x20-0x27, slave 8059A 0x28-0x2f
 * */
void pic_init(uint8_t offset1, uint8_t offset2) {
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

/*
 * Tell PIC interrupt is handled
 * */
void ack_irq(uint8_t irq) {
    if(irq >= 0x28)
        outb(PIC2, PIC_EOI);
    outb(PIC1, PIC_EOI);
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