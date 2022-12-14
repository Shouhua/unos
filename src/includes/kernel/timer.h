#ifndef __TIMER_H__
#define __TIMER_H__

#include "lib/stdint.h"
#include "kernel/isr.h"

typedef void (*timer_callback) ();

typedef struct time_list_node {
	timer_callback callback;
	double sec;	
	uint32_t offset;
	uint32_t t;
	struct time_list_node* next;
} time_list_node_t;

extern uint32_t jiffies;
extern uint16_t frequency_g;
void init_timer(uint16_t frequency);
void register_timer_callback(timer_callback callback, double sec);
void sleep(double sec);

#endif