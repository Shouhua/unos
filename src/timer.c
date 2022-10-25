// from http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html

#include "timer.h"
#include "isr.h"
#include "string.h"
#include "io.h"
#include "serial.h"

#define PIT_TIMER_MAX_HZ 1193180
#define PIT_DATA_PORT0 0x40
#define PIT_DATA_PORT1 0x41
#define PIT_DATA_PORT2 0x42
#define PIT_COMMAND_PORT 0x43

#define PIT_REPEAT 0x36

u32 tick = 0;

static void timer_cb(registers_t regs)
{
	tick++;
	printf("tick: %i\n", tick);
	printf("h/w interrupt: %i\n", regs.int_no);
}

void init_timer(u32 frequency)
{
	printf("initializing timer.\n");
	register_interrupt_handler(IRQ0, &timer_cb);

	u32 divisor = PIT_TIMER_MAX_HZ / frequency;

	outb(PIT_COMMAND_PORT, PIT_REPEAT);

	u8 low = divisor & 0xff;
	u8 high = (divisor >> 8) & 0xff;

	outb(PIT_DATA_PORT0, low);
	outb(PIT_DATA_PORT0, high);
}