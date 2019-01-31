#include "rpi.h"
#include "gpio.h"

#define GPIO_BASE 0x20200000
#define GPSET0  (GPIO_BASE+0x1c)
#define GPCLR0  (GPIO_BASE+0x28)

int gpio_set_function(unsigned pin, unsigned val) {
        if(pin >= 32)
                return -1;
	if((val & 0b111) != val)
		return -1;

        unsigned *gp = ((unsigned *)GPIO_BASE + pin/10);
        unsigned off = (pin%10)*3;

        unsigned v = get32(gp);
                v &= ~(0b111 << off);
                v |= (val << off);
        put32(gp, v);
        return 0;
}

int gpio_set_output(unsigned pin) {
	return gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

int gpio_set_input(unsigned pin) {
	return gpio_set_function(pin, GPIO_FUNC_INPUT);
}

int gpio_set_off(unsigned pin) {
	if(pin >= 32)
		return -1;
	PUT32(GPCLR0, 1 << pin);
	return 0;
}
int gpio_set_on(unsigned pin) {
	if(pin >= 32)
		return -1;
	PUT32(GPSET0, 1 << pin);
	return 0;
}

int gpio_write(unsigned pin, unsigned v) {
	if(v)
		return gpio_set_on(pin);
	else
		return gpio_set_off(pin);
}
