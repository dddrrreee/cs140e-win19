#ifndef __RPI_H__
#define __RPI_H__

void dummy(unsigned);
void reboot(void);

// *(unsigned *)addr = v;
void PUT32(unsigned addr, unsigned v);

// *(unsigned char *)addr = v;
void PUT8(unsigned addr, unsigned v);

// *(unsigned *)addr
unsigned GET32(unsigned addr);

// set pc value to <addr>
void BRANCHTO ( unsigned int addr);


// uart functions
void uart_init ( void );
int uart_getc ( void );
void uart_putc ( unsigned int c );

#endif
