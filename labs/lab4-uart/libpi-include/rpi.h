#ifndef __RPI_H__
#define __RPI_H__

void dummy(unsigned);
void reboot(void);
void clean_reboot(void);

// *(unsigned *)addr = v;
void PUT32(unsigned addr, unsigned v);

// *(unsigned char *)addr = v;
void PUT8(unsigned addr, unsigned v);

// *(unsigned *)addr
unsigned GET32(unsigned addr);

// set pc value to <addr>
void BRANCHTO ( unsigned int addr);

void put32(volatile void *addr, unsigned v);
unsigned get32(const volatile void *addr);

// uart functions
void uart_init ( void );
int uart_getc ( void );
void uart_putc ( unsigned int c );

// should clean this up.
#include "printf.h"
#include "timer.h"

#endif
