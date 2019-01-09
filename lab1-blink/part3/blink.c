/*
 * blink for GPIO16.  Change for GPIO20.
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

// i think we need memory barriers.
void gpio_set_output(unsigned pin) {
        unsigned slot = (pin - 10) * 3;
        *gpio_fsel1 &= ~(0b111 << slot);
        *gpio_fsel1 |= (1 << slot);
}

void gpio_set_on(unsigned pin) {
        *gpio_set0 |= (1 << pin);
}
void gpio_set_off(unsigned pin) {
        *gpio_clr0 |= (1 << pin);
}

// countdown 'ticks' cycles; the asm probably isn't necessary.
void delay(unsigned ticks) {
	while(ticks-- > 0)
		asm("add r1, r1, #0");
}

int notmain ( void ) {
	int led = 20;
	led = 16;

  	gpio_set_output(led);
        while(1) {
                gpio_set_on(led);
                delay(10000000);
                gpio_set_off(led);
                delay(1000000);
        }
	return 0;
}
