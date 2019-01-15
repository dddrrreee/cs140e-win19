/*
 * blink for arbitrary pins.    
 * Implement:
 *	- gpio_set_output;
 *	- gpio_set_on;
 * 	- gpio_set_off.
 *
 *
 * - try deleting volatile.
 * - change makefile to use -O3
 * - get code to work by calling out to a set32 function to set the address.
 * - initialize a structure with all the fields.
 */

// see broadcomm documents for magic addresses.
#define GPIO_BASE 0x20200000
volatile unsigned *gpio_fsel1 = (volatile unsigned *)(GPIO_BASE + 0x04);
volatile unsigned *gpio_set0  = (volatile unsigned *)(GPIO_BASE + 0x1C);
volatile unsigned *gpio_clr0  = (volatile unsigned *)(GPIO_BASE + 0x28);

// XXX might need memory barriers.
void gpio_set_output(unsigned pin) {
	// gpio_fsel1  --- set 'pin' to output.
}

void gpio_set_on(unsigned pin) {
	// use gpio_set0
}
void gpio_set_off(unsigned pin) {
	// use gpio_clr0
}

// countdown 'ticks' cycles; the asm probably isn't necessary.
void delay(unsigned ticks) {
	while(ticks-- > 0)
		asm("add r1, r1, #0");
}

int notmain ( void ) {
	int led = 20;

  	gpio_set_output(led);
        while(1) {
                gpio_set_on(led);
                delay(1000000);
                gpio_set_off(led);
                delay(1000000);
        }
	return 0;
}
