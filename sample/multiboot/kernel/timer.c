// from http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
#include "lib/string.h"
#include "kernel/timer.h"
#include "kernel/isr.h"
#include "kernel/io.h"
#include "kernel/serial.h"

#define PIT_TIMER_MAX_HZ 1193180
#define PIT_DATA_PORT0 0x40
#define PIT_DATA_PORT1 0x41
#define PIT_DATA_PORT2 0x42
#define PIT_COMMAND_PORT 0x43

#define PIT_REPEAT 0x36

uint32_t tick = 0;

void timer_cb(registers_t* regs)
{
	if(tick < 2) {
		tick++;
		printf("tick: %i\n", tick);
		printf("h/w interrupt: %i\n", regs->int_no);
	}
}

void init_timer(uint32_t frequency)
{
	printf("initializing timer.\n");
	register_interrupt_handler(IRQ0, timer_cb);

	uint32_t divisor = PIT_TIMER_MAX_HZ / frequency;

	outb(PIT_COMMAND_PORT, PIT_REPEAT);

	uint8_t low = divisor & 0xff;
	uint8_t high = (divisor >> 8) & 0xff;

	outb(PIT_DATA_PORT0, low);
	outb(PIT_DATA_PORT0, high);
}