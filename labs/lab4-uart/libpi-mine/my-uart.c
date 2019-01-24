#include "rpi.h"
#include "uart.h"
#include "gpio.h"
#include "mem-barrier.h"

// use this if you need memory barriers.
void dev_barrier(void) {
	dmb();
	dsb();
}

void uart_init(void) {
}

int uart_getc(void) {
	return 0;
}
void uart_putc(unsigned c) {
}
