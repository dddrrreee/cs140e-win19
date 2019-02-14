#include "rpi.h"

// as you can see, b/c we have no OS protection, we have to change code
// to prevent it from killing the machine.  we will add more and more
// tricks to prevent this vulnerability.
void notmain(void) {
	// NB: we can't do this b/c the shell already initialized and resetting
	// uart may reset connection to Unix.
	// uart_init();

	// if not working, try just printing characters.
#if 0
	uart_putc('h');
	uart_putc('e');
	uart_putc('l');
	uart_putc('l');
	uart_putc('o');

	uart_putc(' ');

	uart_putc('w');
	uart_putc('o');
	uart_putc('r');
	uart_putc('l');
	uart_putc('d');

	uart_putc('\n');
#endif

	printk("hello world\n");

	return;

	// printk("hello world from address %p\n", (void*)notmain);


	// NB: this is supposed to be a thread_exit().  calling reboot will
	// kill the pi.
	// clean_reboot();
}
