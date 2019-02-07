/* make GPIO pin generate an interrupt when it goes low to high.  */
#include "rpi.h"
#include "timer-interrupt.h"

// use whatever pin you want.
const int pin = 20;

// client has to define this.
void int_handler(unsigned pc) {
	// detect (1) if the gpio event triggered and (2) clear it.
	unimplemented();
}

void OR_IN32(unsigned addr, unsigned val) {
        PUT32(addr, GET32(addr) | val);
}

void dev_barrier(void) {
        dmb();
        dsb();
}

int gpio_event_detected(unsigned pin) {
	if(pin >= 32)
		return -1;
	unimplemented();
}

int gpio_event_clear(unsigned pin) {
	if(pin >= 32)
		return -1;
	unimplemented();
}

int gpio_int_rising_edge(unsigned pin) {
	if(pin >= 32)
		return -1;
	unimplemented();
}

void notmain() {
	uart_init();
	
	printk("about to install handlers\n");
        install_int_handlers();

	dev_barrier();

        // BCM2835 manual, section 7.5
        PUT32(INTERRUPT_DISABLE_1, 0xffffffff);
        PUT32(INTERRUPT_DISABLE_2, 0xffffffff);

	dev_barrier();

	printk("setting up GPIO interrupts for pin: %d\n", pin);
	gpio_set_input(pin);
	gpio_set_pulldown(pin);
	gpio_int_rising_edge(pin);
	// gpio_int_high(pin);

	printk("gonna enable ints globally!\n");


  	system_enable_interrupts();
	printk("enabled!\n");

	unimplemented();

	clean_reboot();
}
