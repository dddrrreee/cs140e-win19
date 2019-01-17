#ifndef __RPI_H__
#define __RPI_H__

#include "gpio.h"

// FIXME: ugly way to strip volatile.
#define put32(x,y) (put32)((void*)x, y)
#define get32(x) (get32)((void*)x)

unsigned (get32)(void *addr);

// *(unsigned *)addr = v
void (put32)(void *addr, unsigned val);

void notmain(void);

void delay(unsigned ticks);

#endif /* __RPI_H__ */
