#ifndef __TIMER_INTERRUPT_H__
#define __TIMER_INTERRUPT_H__

#include "rpi-armtimer.h"
#include "rpi-interrupts.h"
#include "mem-barrier.h"

void timer_interrupt_init(unsigned ncycles);
#endif
