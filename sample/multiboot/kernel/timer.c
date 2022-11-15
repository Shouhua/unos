// from http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
#include "lib/string.h"
#include "kernel/timer.h"
#include "kernel/io.h"
#include "lib/log.h"
#include "kernel/mm/malloc.h"

#define PIT_TIMER_MAX_HZ 1193180
#define PIT_DATA_PORT0 0x40
#define PIT_DATA_PORT1 0x41
#define PIT_DATA_PORT2 0x42
#define PIT_COMMAND_PORT 0x43
#define PIT_REPEAT 0x36

typedef void (*time_callback)();
typedef struct time_list_node {
	time_callback callback;
	uint32_t sec;	
	uint32_t t;
	struct time_list_node* next;
} time_list_node_t;

uint32_t jiffies = 0;
uint16_t frequency_g = 0;
time_list_node_t* head = NULL;

void sleep(int sec) {
	uint32_t end = jiffies + sec * frequency_g;
	while(jiffies < end);
}

void register_timer_callback(timer_callback callback, uint32_t sec) {
	uint32_t jiffy = jiffies + sec * frequency_g;
	time_list_node_t* node = (time_list_node_t*)malloc(sizeof(time_list_node_t));
	node->callback = callback;
	node->sec = sec;
	node->t = jiffy;
	node->next = (time_list_node_t*)head;
	head = node;
}

void timer_handler(__attribute__((unused))registers_t* regs)
{
	jiffies++;
	if(head != NULL) {
		time_list_node_t* current = head;
		do
		{
			if(current->t == jiffies) {
				current->callback();
			}
			current = current->next;
		} while (current != NULL && current->next != NULL);
	}
}

void init_timer(uint16_t frequency)
{
	frequency_g = frequency;
	#ifdef DEBUG 
		INFO("[Timer] Initializing timer.\n");
	#endif

	register_interrupt_handler(IRQ0, timer_handler);
	uint32_t divisor = PIT_TIMER_MAX_HZ / frequency;
	outb(PIT_COMMAND_PORT, PIT_REPEAT);
	uint16_t low = divisor & 0xff;
	uint16_t high = (divisor >> 8) & 0xff;
	outb(PIT_DATA_PORT0, low);
	outb(PIT_DATA_PORT0, high);
}