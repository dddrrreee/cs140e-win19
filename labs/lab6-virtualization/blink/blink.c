#include "rpi.h"

static const unsigned ms = 50;

void notmain(void) {
        int led = 21;

	printf("about to do blink!\n");
	gpio_set_output(led);

	for(int i = 0; i < 20; i++) { 
                gpio_write(led,1);
		delay_ms(ms);
                gpio_write(led,0);
		delay_ms(ms);
        }
	printf("done!\n");
	clean_reboot();
}

