#ifndef __RPI_H__
#define __RPI_H__

#include "gpio.h"

// return *(volatile unsigned *)addr
unsigned get32(volatile void *addr);

// *(volatile unsigned *)addr = v
void put32(volatile void *addr, unsigned val);

void notmain(void);

void delay(unsigned ticks);

#endif /* __RPI_H__ */
