#include "rpi.h"

static const unsigned ms = 50;

void notmain(void) {
        int led = 21;

	printf("about to do blink!\n");
	gpio_set_output(led);

	for(int i = 0; i < 20; i++) { 
                gpio_set_on(led);
		delay_ms(ms);
                gpio_set_off(led);
		delay_ms(ms);
        }
	printf("done!\n");
	clean_reboot();
}

