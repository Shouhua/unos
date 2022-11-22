#ifndef __TIMER_H__
#define __TIMER_H__

#include "lib/stdint.h"
#include "kernel/isr.h"

typedef void (*timer_callback) ();
extern uint32_t jiffies;
extern uint16_t frequency_g;
void init_timer(uint16_t frequency);
void register_timer_callback(timer_callback callback, double sec);
void sleep(double sec);

#endif