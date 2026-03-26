#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint64_t ticks;

void init_timer(uint32_t frequency);

#endif